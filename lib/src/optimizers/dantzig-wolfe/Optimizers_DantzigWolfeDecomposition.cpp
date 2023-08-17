//
// Created by henri on 24/03/23.
//
#include <cassert>
#include "optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.h"
#include "modeling/objects/Versions.h"
#include "modeling/expressions/operations/operators.h"

idol::Optimizers::DantzigWolfeDecomposition::DantzigWolfeDecomposition(const Model& t_original_formulation,
                                                                 const Annotation<Ctr, unsigned int>& t_constraint_flag,
                                                                 const Annotation<Var, unsigned int>& t_variable_flag,
                                                                 Model *t_master_problem,
                                                                 const std::vector<Model *> &t_subproblems,
                                                                 std::vector<Column> t_generation_patterns,
                                                                 const OptimizerFactory& t_pricing_optimizer)
     : Optimizers::ColumnGeneration(t_original_formulation, t_master_problem, t_subproblems, std::move(t_generation_patterns)),
       m_var_annotation(t_variable_flag),
       m_ctr_annotation(t_constraint_flag),
       m_pricing_optimizer(t_pricing_optimizer.clone()) {

}

double idol::Optimizers::DantzigWolfeDecomposition::get_var_primal(const Var &t_var) const {

    const unsigned int subproblem_id = t_var.get(m_var_annotation);

    if (subproblem_id != MasterId) {
        return get_subproblem_primal_value(t_var, subproblem_id);
    }

    return ColumnGeneration::get_var_primal(t_var);
}

double idol::Optimizers::DantzigWolfeDecomposition::get_subproblem_primal_value(const Var &t_var, unsigned int t_subproblem_id) const {

    double result = 0;
    for (const auto& [alpha, generator] : m_subproblems[t_subproblem_id].m_present_generators) {
        const double alpha_val = m_master->get_var_primal(alpha);
        if (alpha_val > 0) {
            result += alpha_val * generator.get(t_var);
        }
    }

    return result;
}

void idol::Optimizers::DantzigWolfeDecomposition::set_subproblem_lower_bound(const Var &t_var, unsigned int t_subproblem_id, double t_value) {

    auto& subproblem = m_subproblems[t_subproblem_id];

    subproblem.remove_column_if([&](const Var& t_object, const Solution::Primal& t_generator) {
        //return true;
        return t_generator.get(t_var) < t_value;
    });

    if (!m_branching_on_master) {
        subproblem.m_model->set_var_lb(t_var, t_value);
        return;
    }

    apply_subproblem_bound_on_master(true, t_var, t_subproblem_id, t_value);

}

void idol::Optimizers::DantzigWolfeDecomposition::set_subproblem_upper_bound(const Var &t_var, unsigned int t_subproblem_id, double t_value) {

    auto& subproblem = m_subproblems[t_subproblem_id];

    subproblem.remove_column_if([&](const Var& t_object, const Solution::Primal& t_generator) {
        //return true;
        return t_generator.get(t_var) > t_value;
    });

    if (!m_branching_on_master) {
        subproblem.m_model->set_var_ub(t_var, t_value);
        return;
    }

    apply_subproblem_bound_on_master(false, t_var, t_subproblem_id, t_value);

}

void idol::Optimizers::DantzigWolfeDecomposition::apply_subproblem_bound_on_master(bool t_is_lb, const Var &t_var, unsigned int t_subproblem_id, double t_value) {

    auto& subproblem = m_subproblems[t_subproblem_id];

    auto& applied_bounds = t_is_lb ? m_lower_bound_constraints : m_upper_bound_constraints;
    const auto it = applied_bounds.find(t_var);

    if (it == applied_bounds.end()) { // Create a new constraint

        auto expanded = expand_subproblem_variable(t_var, t_subproblem_id);
        const CtrType type = t_is_lb ? GreaterOrEqual : LessOrEqual;

        Ctr bound_constraint(m_master->env(), Equal, 0);

        m_master->add(bound_constraint, TempCtr(::idol::Row(expanded, t_value), type));
        subproblem.m_generation_pattern.linear().set(bound_constraint, !t_var);

        applied_bounds.emplace(t_var, bound_constraint);

        return;
    }

    const auto& current_rhs = m_master->get_ctr_row(it->second).rhs();

    if (!current_rhs.is_numerical()) {
        throw Exception("Unexpected RHS with non-numerical terms.");
    }

    if (equals(t_value, current_rhs.numerical(), Tolerance::Integer)) { // Remove existing constraint for it is not useful anymore

        subproblem.m_generation_pattern.linear().remove(it->second);
        m_master->remove(it->second);
        applied_bounds.erase(it);

        return;
    }

    m_master->set_ctr_rhs(it->second, t_value);

}

idol::LinExpr<idol::Var> idol::Optimizers::DantzigWolfeDecomposition::expand_subproblem_variable(const Var &t_var, unsigned int t_subproblem_id) {

    LinExpr<Var> result;

    for (const auto& [alpha, generator] : m_subproblems[t_subproblem_id].m_present_generators) {
        result += generator.get(t_var) * alpha;
    }

    return result;
}

void idol::Optimizers::DantzigWolfeDecomposition::update_var_lb(const Var &t_var) {

    const unsigned int subproblem_id = t_var.get(m_var_annotation);

    if (subproblem_id != MasterId) {
        set_subproblem_lower_bound(t_var, subproblem_id, parent().get_var_lb(t_var));
        return;
    }

    ColumnGeneration::update_var_lb(t_var);
}

void idol::Optimizers::DantzigWolfeDecomposition::update_var_ub(const Var &t_var) {

    const unsigned int subproblem_id = t_var.get(m_var_annotation);

    if (subproblem_id != MasterId) {
        set_subproblem_upper_bound(t_var, subproblem_id, parent().get_var_ub(t_var));
        return;
    }

    ColumnGeneration::update_var_ub(t_var);
}

void idol::Optimizers::DantzigWolfeDecomposition::set_objective(Expr<Var, Var> &&t_objective) {

    const unsigned int n_subproblems = m_subproblems.size();

    Expr<Var, Var> master_obj = std::move(t_objective.constant());
    std::vector<Constant> pricing_obj(n_subproblems);

    for (auto [var, coeff] : t_objective.linear()) {
        const unsigned int subproblem_id = var.get(m_var_annotation);
        if (subproblem_id == MasterId) {
            master_obj += coeff * var;
        } else {
            if (!coeff.is_numerical()) {
                throw Exception("Could not handle non-numerical objective coefficient as generation pattern.");
            }
            pricing_obj[subproblem_id] += coeff.numerical() * !var;
        }
    }

    m_master->set_obj_expr(std::move(master_obj));

    for (unsigned int k = 0 ; k < n_subproblems ; ++k) {
        m_subproblems[k].update_generation_pattern_objective(std::move(pricing_obj[k]));
    }

}

void idol::Optimizers::DantzigWolfeDecomposition::add(const Var &t_var) {

    const auto subproblem_id = t_var.get(m_var_annotation);

    if (subproblem_id == MasterId) {
        ColumnGeneration::add(t_var);
        return;
    }

    if (m_subproblems.size() <= subproblem_id) {

        auto& env = m_master->env();

        for (unsigned int i = subproblem_id ; i <= subproblem_id ; ++i) {

            auto* model = new Model(env);

            auto convex = m_master->add_ctr(Row(0, 1), Equal);

            Column column;
            column.linear().set(convex, 1);

            model->use(*m_pricing_optimizer);

            add_subproblem(model, std::move(column));

        }

    }

    m_subproblems[subproblem_id].m_model->add(t_var);

}

void idol::Optimizers::DantzigWolfeDecomposition::add(const Ctr &t_ctr) {

    const auto subproblem_id = t_ctr.get(m_ctr_annotation);

    const auto& row = parent().get_ctr_row(t_ctr);
    const auto type = parent().get_ctr_type(t_ctr);

    if (subproblem_id != MasterId) {
        m_subproblems[subproblem_id].m_model->add(t_ctr, TempCtr(Row(row), type));
        return;
    }

    const unsigned int n_subproblems = m_subproblems.size();

    Row master_row;
    std::vector<Constant> pricing_pattern(n_subproblems);

    master_row.rhs() = row.rhs();

    for (auto [var, coeff] : row.linear()) {
        const unsigned int var_subproblem_id = var.get(m_var_annotation);

        if (var_subproblem_id == MasterId) {
            master_row.linear() += coeff * var;
        } else {
            if (!coeff.is_numerical()) {
                throw Exception("Could not handle non-numerical objective coefficient as generation pattern.");
            }
            pricing_pattern[var_subproblem_id] += coeff.numerical() * !var;
        }

    }

    // Remove columns

    double master_lhs = 0;

    for (const auto& [var, constant] : master_row.linear()) {
        master_lhs += constant.numerical() * m_master->get_var_primal(var);
    }

    const double master_rhs = master_row.rhs().numerical();

    for (auto& subproblem : m_subproblems) {

        if (pricing_pattern[subproblem.m_index].is_zero()) {
            continue;
        }

        subproblem.remove_column_if([&](const auto& t_alpha, const auto& t_generator) -> double {
            const double value = master_lhs + pricing_pattern[subproblem.m_index].fix(t_generator) - master_rhs;
            switch (type) {
                case LessOrEqual: return value > 0;
                case GreaterOrEqual: return value < 0;
                default:;
            }
            return !equals(value, 0., 1e-6);
        });

        subproblem.m_generation_pattern.linear().set(t_ctr, std::move(pricing_pattern[subproblem.m_index]));

    }

    // Add cut
    m_master->add(t_ctr, TempCtr(Row(std::move(master_row)), type));

}
