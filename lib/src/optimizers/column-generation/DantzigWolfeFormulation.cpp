//
// Created by henri on 31.10.23.
//
#include <utility>

#include "idol/optimizers/column-generation/DantzigWolfeFormulation.h"
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

    init_sub_problems(n_sub_problems);
    init_generation_patterns(n_sub_problems);

    dispatch_variables(t_original_formulation);
    dispatch_constraints(t_original_formulation);
    dispatch_objective_function(t_original_formulation);

}

unsigned int idol::DantzigWolfe::Formulation::compute_n_sub_problems(const Model& t_original_formulation) {

    std::optional<unsigned int> n_sub_problems;

    for (const auto& ctr : t_original_formulation.ctrs()) {

        const unsigned int sub_problem_id = ctr.get(m_decomposition_by_ctr);

        if (!n_sub_problems.has_value()) {
            n_sub_problems = sub_problem_id;
            continue;
        }

        if (sub_problem_id != MasterId && sub_problem_id >= n_sub_problems.value()) {
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

void idol::DantzigWolfe::Formulation::init_sub_problems(unsigned int t_n_sub_problems) {

    auto& env = m_master.env();

    m_sub_problems.reserve(t_n_sub_problems);
    for (unsigned int i = 0 ; i < t_n_sub_problems ; ++i) {
        m_sub_problems.emplace_back(env);
    }

}

void idol::DantzigWolfe::Formulation::init_generation_patterns(unsigned int t_n_sub_problems) {

    m_generation_patterns = std::vector<Column>(t_n_sub_problems);

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

void
idol::DantzigWolfe::Formulation::add_aggregation_constraint(unsigned int t_sub_problem_id,
                                                            double t_lower_multiplicity,
                                                            double t_upper_multiplicity) {



}
