//
// Created by henri on 31.10.23.
//
#include <utility>

#include "idol/optimizers/dantzig-wolfe/Formulation.h"
#include "idol/modeling/objects/Env.h"
#include "idol/modeling/expressions/operations/operators.h"

idol::DantzigWolfe::Formulation::Formulation(const Model &t_original_formulation,
                                             Annotation<Ctr, unsigned int> t_decomposition)
    : m_decomposition_by_ctr(std::move(t_decomposition)),
      m_decomposition_by_var(t_original_formulation.env(), m_decomposition_by_ctr.name(), MasterId),
      m_master(t_original_formulation.env())
{

    unsigned int n_sub_problems = compute_n_sub_problems(t_original_formulation);

    set_decomposition_by_var(t_original_formulation);

    initialize_sub_problems(n_sub_problems);
    initialize_generation_patterns(n_sub_problems);
    initialize_pools(n_sub_problems);
    initialize_present_generators(n_sub_problems);

    dispatch_variables(t_original_formulation);
    dispatch_constraints(t_original_formulation);
    dispatch_objective_function(t_original_formulation);

}

unsigned int idol::DantzigWolfe::Formulation::compute_n_sub_problems(const Model& t_original_formulation) {

    std::optional<unsigned int> n_sub_problems;

    for (const auto& ctr : t_original_formulation.ctrs()) {

        const unsigned int sub_problem_id = ctr.get(m_decomposition_by_ctr);

        if (sub_problem_id == MasterId) {
            continue;
        }

        if (!n_sub_problems.has_value()) {
            n_sub_problems = sub_problem_id;
            continue;
        }

        if (sub_problem_id >= n_sub_problems.value()) {
            n_sub_problems = sub_problem_id;
        }

    }

    return n_sub_problems.has_value() ? ++n_sub_problems.value() : 0; // ++ because indices start at 0
}

void idol::DantzigWolfe::Formulation::set_decomposition_by_var(const Model& t_original_formulation) {

    const auto set_decomposition_by_var = [this](const Var &t_var, unsigned int t_sub_problem_id) {

        if (t_sub_problem_id == MasterId) { return; }

        const unsigned int current_sub_problem_id = t_var.get(m_decomposition_by_var);

        if (current_sub_problem_id != MasterId && current_sub_problem_id != t_sub_problem_id) {
            throw Exception("Impossible decomposition.");
        }

        t_var.set(m_decomposition_by_var, t_sub_problem_id);
    };

    for (const auto &ctr: t_original_formulation.ctrs()) {

        const unsigned int sub_problem_id = ctr.get(m_decomposition_by_ctr);

        const auto &row = t_original_formulation.get_ctr_row(ctr);

        for (const auto &[var, constant]: row.linear()) {
            set_decomposition_by_var(var, sub_problem_id);
        }

        for (const auto &[var1, var2, constant]: row.quadratic()) {
            set_decomposition_by_var(var1, sub_problem_id);
            set_decomposition_by_var(var2, sub_problem_id);
        }

    }

}

void idol::DantzigWolfe::Formulation::initialize_sub_problems(unsigned int t_n_sub_problems) {

    auto& env = m_master.env();

    m_sub_problems.reserve(t_n_sub_problems);
    for (unsigned int i = 0 ; i < t_n_sub_problems ; ++i) {
        m_sub_problems.emplace_back(env);
    }

}

void idol::DantzigWolfe::Formulation::initialize_generation_patterns(unsigned int t_n_sub_problems) {

    m_generation_patterns = std::vector<Column>(t_n_sub_problems);

}

void idol::DantzigWolfe::Formulation::initialize_pools(unsigned int t_n_sub_problems) {

    m_pools = std::vector<GeneratorPool<Var>>(t_n_sub_problems);

}

void idol::DantzigWolfe::Formulation::initialize_present_generators(unsigned int t_n_sub_problems) {

    m_present_generators = std::vector<PresentGeneratorsList>(t_n_sub_problems);

}

void idol::DantzigWolfe::Formulation::dispatch_variables(const Model& t_original_formulation) {

    for (const auto& var : t_original_formulation.vars()) {

        const double lb = t_original_formulation.get_var_lb(var);
        const double ub = t_original_formulation.get_var_ub(var);
        const VarType type = t_original_formulation.get_var_type(var);

        auto& model = get_model(var);

        if (model.has(var)) {
            continue;
        }

        model.add(var, TempVar(lb, ub, type, Column()));

    }

}

void idol::DantzigWolfe::Formulation::dispatch_constraints(const idol::Model &t_original_formulation) {

    if (m_sub_problems.empty()) {
        return;
    }

    for (const auto& ctr : t_original_formulation.ctrs()) {

        const unsigned int sub_problem_id = ctr.get(m_decomposition_by_ctr);
        const auto& row = t_original_formulation.get_ctr_row(ctr);
        const auto type = t_original_formulation.get_ctr_type(ctr);

        if (sub_problem_id == MasterId) {
            dispatch_linking_constraint(ctr, row, type);
        } else {
            m_sub_problems[sub_problem_id].add(ctr, TempCtr(Row(row), type));
        }

    }

}

void idol::DantzigWolfe::Formulation::dispatch_linking_constraint(const idol::Ctr &t_original_ctr,
                                                                  const idol::Row &t_row,
                                                                  idol::CtrType t_type) {

    const unsigned int n_sub_problems = m_sub_problems.size();

    auto [master_part, sub_problem_parts] = decompose_expression(t_row.linear(), t_row.quadratic());

    m_master.add(t_original_ctr, TempCtr(Row(std::move(master_part), t_row.rhs()), t_type));

    for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
        m_generation_patterns[i].linear().set(t_original_ctr, std::move(sub_problem_parts[i]));
    }


}

std::pair<idol::Expr<idol::Var, idol::Var>, std::vector<idol::Constant>>
idol::DantzigWolfe::Formulation::decompose_expression(const LinExpr<Var> &t_linear, const QuadExpr<Var, Var>& t_quadratic) {

    const unsigned int n_sub_problems = m_sub_problems.size();

    Expr<Var, Var> master_part;
    std::vector<Constant> sub_problem_parts(n_sub_problems);

    for (const auto& [var, constant] : t_linear) {

        const unsigned int sub_problem_id = var.get(m_decomposition_by_var);

        if (sub_problem_id == MasterId) {
            master_part += constant * var;
            continue;
        }

        if (!constant.is_numerical()) {
            throw Exception("Cannot have linking expression with parametrized coefficients for sub-problem's variable.");
        }

        sub_problem_parts[sub_problem_id] += constant.numerical() * !var;

    }

    for (const auto& [var1, var2, constant] : t_quadratic) {

        const unsigned int sub_problem_id1 = var1.get(m_decomposition_by_var);
        const unsigned int sub_problem_id2 = var2.get(m_decomposition_by_var);

        if (sub_problem_id1 != sub_problem_id2) {
            throw Exception("Impossible decomposition.");
        }

        if (sub_problem_id1 == MasterId) {
            master_part += constant * var1 * var2;
            continue;
        }

        if (!constant.is_numerical()) {
            throw Exception("Cannot have linking expression with parametrized coefficients for sub-problem's variable.");
        }

        sub_problem_parts[sub_problem_id1] += constant.numerical() * (!var1 * !var2);

    }

    return std::make_pair(
            std::move(master_part),
            std::move(sub_problem_parts)
    );

}

void idol::DantzigWolfe::Formulation::dispatch_objective_function(const idol::Model &t_original_formulation) {

    const unsigned int n_subproblems = m_sub_problems.size();

    const auto& objective = t_original_formulation.get_obj_expr();

    auto [master_part, sub_problem_parts] = decompose_expression(objective.linear(), objective.quadratic());

    master_part += objective.constant();

    m_master.set_obj_expr(std::move(master_part));

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        m_generation_patterns[i].set_obj(std::move(sub_problem_parts[i]));
    }

}

idol::Model &idol::DantzigWolfe::Formulation::get_model(const idol::Var &t_var) {
    const unsigned int sub_problem_id = t_var.get(m_decomposition_by_var);
    return sub_problem_id == MasterId ? m_master : m_sub_problems[sub_problem_id];
}

const idol::Model &idol::DantzigWolfe::Formulation::get_model(const idol::Var &t_var) const {
    const unsigned int sub_problem_id = t_var.get(m_decomposition_by_var);
    return sub_problem_id == MasterId ? m_master : m_sub_problems[sub_problem_id];
}

idol::Model &idol::DantzigWolfe::Formulation::get_model(const idol::Ctr &t_ctr) {
    const unsigned int sub_problem_id = t_ctr.get(m_decomposition_by_ctr);
    return sub_problem_id == MasterId ? m_master : m_sub_problems[sub_problem_id];
}

const idol::Model &idol::DantzigWolfe::Formulation::get_model(const idol::Ctr &t_ctr) const {
    const unsigned int sub_problem_id = t_ctr.get(m_decomposition_by_ctr);
    return sub_problem_id == MasterId ? m_master : m_sub_problems[sub_problem_id];
}

void idol::DantzigWolfe::Formulation::add_aggregation_constraint(unsigned int t_sub_problem_id,
                                                                 double t_lower_multiplicity,
                                                                 double t_upper_multiplicity) {

    auto& env = m_master.env();

    Ctr lower(env, GreaterOrEqual, t_lower_multiplicity);
    m_master.add(lower);
    m_generation_patterns[t_sub_problem_id].linear().set(lower, 1);

    Ctr upper(env, LessOrEqual, t_upper_multiplicity);
    m_master.add(upper);
    m_generation_patterns[t_sub_problem_id].linear().set(upper, 1);

}

void idol::DantzigWolfe::Formulation::generate_column(unsigned int t_sub_problem_id,
                                                      idol::Solution::Primal t_generator) {

    auto alpha = m_master.add_var(0.,
                                Inf,
                                Continuous,
                                m_generation_patterns[t_sub_problem_id].fix(t_generator)
                                );

    auto& pool = m_pools[t_sub_problem_id];
    auto& present_generators = m_present_generators[t_sub_problem_id];

    pool.add(alpha, std::move(t_generator));
    present_generators.emplace_back(alpha, pool.last_inserted());

}

double idol::DantzigWolfe::Formulation::compute_reduced_cost(unsigned int t_sub_problem_id,
                                                             const idol::Solution::Dual &t_master_dual,
                                                             const idol::Solution::Primal &t_generator) {

    double result = 0.;

    const auto generation_pattern = m_generation_patterns[t_sub_problem_id];

    for (const auto &[ctr, constant] : generation_pattern.linear()) {
        result += constant.numerical() * -t_master_dual.get(ctr);
        for (const auto &[param, coefficient] : constant.linear()) {
            result += -t_master_dual.get(ctr) * coefficient * t_generator.get(param.as<Var>());
        }
        for (const auto &[pair, coefficient] : constant.quadratic()) {
            result += -t_master_dual.get(ctr) * coefficient * t_generator.get(pair.first.as<Var>()) * t_generator.get(pair.second.as<Var>());
        }
    }

    for (const auto &[param, coefficient] : generation_pattern.obj().linear()) {
        result += coefficient * t_generator.get(param.as<Var>());
    }

    for (const auto &[pair, coefficient] : generation_pattern.obj().quadratic()) {
        result += coefficient * t_generator.get(pair.first.as<Var>()) * t_generator.get(pair.second.as<Var>());
    }

    return result;

}

void idol::DantzigWolfe::Formulation::update_sub_problem_objective(unsigned int t_sub_problem_id,
                                                                   const idol::Solution::Dual &t_master_dual,
                                                                   bool t_use_farkas) {

    Expr<Var, Var> objective;

    const auto generation_pattern = m_generation_patterns[t_sub_problem_id];

    for (const auto &[ctr, constant] : generation_pattern.linear()) {

        objective += constant.numerical() * -t_master_dual.get(ctr);

        for (const auto &[param, coefficient] : constant.linear()) {

            const double cost = -t_master_dual.get(ctr) * coefficient;
            if (!equals(cost, 0., Tolerance::Sparsity)) {
                objective += cost * param.as<Var>();
            }

        }

        for (const auto &[pair, coefficient] : constant.quadratic()) {

            const double cost = -t_master_dual.get(ctr) * coefficient;
            if (!equals(cost, 0., Tolerance::Sparsity)) {
                objective += cost * pair.first.as<Var>() * pair.second.as<Var>();
            }

        }
    }

    if (!t_use_farkas) {

        for (const auto &[param, coefficient] : generation_pattern.obj().linear()) {

            if (!equals(coefficient, 0., Tolerance::Sparsity)) {
                objective += coefficient * param.as<Var>();
            }

        }

        for (const auto &[pair, coefficient] : generation_pattern.obj().quadratic()) {

            if (!equals(coefficient, 0., Tolerance::Sparsity)) {
                objective += coefficient * pair.first.as<Var>() * pair.second.as<Var>();
            }

        }

    }

    m_sub_problems[t_sub_problem_id].set_obj_expr(std::move(objective));

}

double idol::DantzigWolfe::Formulation::get_original_space_var_primal(const idol::Var &t_var,
                                                                      const idol::Solution::Primal &t_master_primal) const {

    const unsigned int sub_problem_id = t_var.get(m_decomposition_by_var);

    if (sub_problem_id == MasterId) {
        return t_master_primal.get(t_var);
    }

    double result = 0;
    for (const auto& [alpha, generator] : m_present_generators[sub_problem_id]) {
        const double alpha_val = t_master_primal.get(alpha);
        if (alpha_val > Tolerance::Sparsity) {
            result += alpha_val * generator.get(t_var);
        }
    }

    return result;

}

void idol::DantzigWolfe::Formulation::update_var_lb(const idol::Var &t_var, double t_lb, bool t_hard) {

    const unsigned int sub_problem_id = t_var.get(m_decomposition_by_var);

    if (sub_problem_id == MasterId) {
        m_master.set_var_lb(t_var, t_lb);
        return;
    }

    remove_column_if(sub_problem_id, [&](const Var& t_object, const Solution::Primal& t_generator) {
        //return true;
        return t_generator.get(t_var) < t_lb;
    });

    if (t_hard) {
        sub_problem(sub_problem_id).set_var_lb(t_var, t_lb);
        return;
    }

    apply_sub_problem_bound_on_master(true, t_var, sub_problem_id, t_lb);

}

void idol::DantzigWolfe::Formulation::update_var_ub(const idol::Var &t_var, double t_ub, bool t_hard) {

    const unsigned int sub_problem_id = t_var.get(m_decomposition_by_var);

    if (sub_problem_id == MasterId) {
        m_master.set_var_ub(t_var, t_ub);
        return;
    }

    remove_column_if(sub_problem_id, [&](const Var& t_object, const Solution::Primal& t_generator) {
        // return true;
        return t_generator.get(t_var) > t_ub;
    });

    if (t_hard) {
        sub_problem(sub_problem_id).set_var_ub(t_var, t_ub);
        return;
    }

    apply_sub_problem_bound_on_master(false, t_var, sub_problem_id, t_ub);

}

void idol::DantzigWolfe::Formulation::apply_sub_problem_bound_on_master(bool t_is_lb,
                                                                        const idol::Var &t_var,
                                                                        unsigned int t_sub_problem_id, double t_value) {

    auto& applied_bounds = t_is_lb ? m_soft_branching_lower_bound_constraints : m_soft_branching_upper_bound_constraints;
    const auto it = applied_bounds.find(t_var);

    if (it == applied_bounds.end()) { // Create a new constraint

        auto expanded = reformulate_sub_problem_variable(t_var, t_sub_problem_id);
        const CtrType type = t_is_lb ? GreaterOrEqual : LessOrEqual;

        Ctr bound_constraint(m_master.env(), Equal, 0);

        m_master.add(bound_constraint, TempCtr(Row(expanded, t_value), type));
        m_generation_patterns[t_sub_problem_id].linear().set(bound_constraint, !t_var);

        applied_bounds.emplace(t_var, bound_constraint);

        return;
    }

    m_master.set_ctr_rhs(it->second, t_value);

}

idol::LinExpr<idol::Var> idol::DantzigWolfe::Formulation::reformulate_sub_problem_variable(const idol::Var &t_var,
                                                                                           unsigned int t_sub_problem_id) {

    LinExpr<Var> result;

    for (const auto& [alpha, generator] : m_present_generators[t_sub_problem_id]) {
        result += generator.get(t_var) * alpha;
    }

    return result;
}

void idol::DantzigWolfe::Formulation::remove_column_if(unsigned int t_sub_problem_id,
                       const std::function<bool(const Var &, const Solution::Primal &)> &t_indicator_for_removal) {

    auto& present_generators = m_present_generators[t_sub_problem_id];
    auto it = present_generators.begin();
    const auto end = present_generators.end();

    while (it != end) {
        const auto& [column_variable, ptr_to_column] = *it;
        if (t_indicator_for_removal(column_variable, ptr_to_column)) {
            m_master.remove(column_variable);
            it = present_generators.erase(it);
        } else {
            ++it;
        }
    }

}

void idol::DantzigWolfe::Formulation::update_obj(const idol::Expr<idol::Var, idol::Var> &t_expr) {

    const unsigned int n_sub_problems = m_sub_problems.size();

    auto [master_part, sub_problem_parts] = decompose_expression(t_expr.linear(), t_expr.quadratic());

    master_part += t_expr.constant();

    m_master.set_obj_expr(std::move(master_part));

    for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {

        for (const auto& [var, generator] : m_present_generators[i]) {
            m_master.set_var_obj(var, sub_problem_parts[i].fix(generator));
        }

        m_generation_patterns[i].set_obj(std::move(sub_problem_parts[i]));

    }

}

void idol::DantzigWolfe::Formulation::clean_up(unsigned int t_sub_problem_id, double t_ratio) {

    auto& pool = m_pools[t_sub_problem_id];
    auto& present_generators = m_present_generators[t_sub_problem_id];
    const auto n_to_remove = (unsigned int) (pool.size() * (1 - t_ratio));
    unsigned int n_removed = 0;

    present_generators.clear();

    for (auto it = pool.values().begin(), end = pool.values().end() ; it != end ; ) {

        const bool is_already_in_master = m_master.has(it->first);
        const bool done_removing = n_removed >= n_to_remove;

        if (done_removing) {

            if (is_already_in_master) {
                present_generators.emplace_back(it->first, it->second);
            }

            ++it;
            continue;

        }

        if (is_already_in_master) {

            if (m_master.get_var_primal(it->first) > 0) {

                present_generators.emplace_back(it->first, it->second);
                ++it;
                continue;

            }

            m_master.remove(it->first);

        }

        it = pool.erase(it);
        ++n_removed;

    }

}

void idol::DantzigWolfe::Formulation::add(const idol::Var &t_var,
                                          double t_lb,
                                          double t_ub,
                                          idol::VarType t_type,
                                          const idol::Column &t_column) {

    const auto sub_problem_id = t_var.get(m_decomposition_by_var);

    if (sub_problem_id == MasterId) {
        m_master.add(t_var, TempVar(t_lb, t_ub, t_type, Column(t_column)));
        return;
    }

    m_sub_problems[sub_problem_id].add(t_var, TempVar(t_lb, t_ub, t_type, Column(t_column)));

}

void idol::DantzigWolfe::Formulation::add(const idol::Ctr &t_ctr, idol::CtrType t_type, const idol::Row &t_row) {

    const auto sub_problem_id = t_ctr.get(m_decomposition_by_ctr);

    if (sub_problem_id != MasterId) {
        m_sub_problems[sub_problem_id].add(t_ctr, TempCtr(Row(t_row), t_type));
        remove_column_if(sub_problem_id, [&](const Var& t_alpha, const Solution::Primal& t_generator) {
            return t_row.is_violated(t_generator, t_type);
        });
        return;
    }

    const unsigned int n_sub_problems = m_sub_problems.size();
    auto [master_part, sub_problem_parts] = decompose_expression(t_row.linear(), t_row.quadratic());

    for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
        for (const auto& [var, generator] : m_present_generators[i]) {
            master_part += sub_problem_parts[i].fix(generator) * var; // Adds exising generator to constraint
        }
        m_generation_patterns[i].linear().set(t_ctr, std::move(sub_problem_parts[i]));
    }

    m_master.add(t_ctr, TempCtr(Row(master_part, t_row.rhs()), t_type));

}

void idol::DantzigWolfe::Formulation::remove(const idol::Var &t_var) {

    const auto sub_problem_id = t_var.get(m_decomposition_by_var);

    if (sub_problem_id != MasterId) {
        m_generation_patterns[sub_problem_id].obj().set(!t_var, 0.);
        m_sub_problems[sub_problem_id].remove(t_var);
        return;
    }

    m_master.remove(t_var);

}

void idol::DantzigWolfe::Formulation::remove(const idol::Ctr &t_ctr) {

    const auto sub_problem_id = t_ctr.get(m_decomposition_by_ctr);

    if (sub_problem_id != MasterId) {
        m_sub_problems[sub_problem_id].remove(t_ctr);
        return;
    }

    m_master.remove(t_ctr);
    m_generation_patterns[sub_problem_id].linear().set(t_ctr, 0.);

}

unsigned int idol::DantzigWolfe::Formulation::get_n_present_generators() const {

    unsigned int result = 0;

    for (unsigned int i = 0, n = n_sub_problems() ; i < n ; ++i) {
        result += m_present_generators[i].size();
    }

    return result;
}
