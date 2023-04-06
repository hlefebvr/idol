//
// Created by henri on 24/03/23.
//
#include "optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.h"
#include "modeling/objects/Versions.h"
#include "modeling/expressions/operations/operators.h"

Optimizers::DantzigWolfeDecomposition::DantzigWolfeDecomposition(const Model& t_original_formulation,
                                                                 const Annotation<Ctr, unsigned int>& t_constraint_flag,
                                                                 const Annotation<Var, unsigned int>& t_variable_flag,
                                                                 Model *t_master_problem,
                                                                 const std::vector<Model *> &t_subproblems,
                                                                 std::vector<Column> t_generation_patterns)
     : Optimizers::ColumnGeneration(t_original_formulation, t_master_problem, t_subproblems, std::move(t_generation_patterns)),
       m_variable_flag(t_variable_flag),
       m_constraint_flag(t_constraint_flag) {

}

double Optimizers::DantzigWolfeDecomposition::get_var_val(const Var &t_var) const {

    const unsigned int subproblem_id = t_var.get(m_variable_flag);

    if (subproblem_id != MasterId) {
        return get_subproblem_primal_value(t_var, subproblem_id);
    }

    return ColumnGeneration::get_var_val(t_var);
}

double Optimizers::DantzigWolfeDecomposition::get_subproblem_primal_value(const Var &t_var, unsigned int t_subproblem_id) const {

    double result = 0;
    for (const auto& [alpha, generator] : m_subproblems[t_subproblem_id].m_present_generators) {
        const double alpha_val = m_master->get_var_val(alpha);
        if (alpha_val > 0) {
            result += alpha_val * generator.get(t_var);
        }
    }

    return result;
}

void Optimizers::DantzigWolfeDecomposition::set_subproblem_lower_bound(const Var &t_var, unsigned int t_subproblem_id, double t_value) {

    auto& subproblem = m_subproblems[t_subproblem_id];

    subproblem.remove_column_if([&](const Var& t_object, const Solution::Primal& t_generator) {
        //return true;
        return t_generator.get(t_var) < t_value;
    });

    if (!m_branching_on_master) {
        subproblem.m_model->set_var_lb(t_var, t_value);
        return;
    }

    apply_subproblem_bound_on_master(::Attr::Var::Lb, t_var, t_subproblem_id, t_value);

}

void Optimizers::DantzigWolfeDecomposition::set_subproblem_upper_bound(const Var &t_var, unsigned int t_subproblem_id, double t_value) {

    auto& subproblem = m_subproblems[t_subproblem_id];

    subproblem.remove_column_if([&](const Var& t_object, const Solution::Primal& t_generator) {
        //return true;
        return t_generator.get(t_var) > t_value;
    });

    if (!m_branching_on_master) {
        subproblem.m_model->set_var_ub(t_var, t_value);
        return;
    }

    apply_subproblem_bound_on_master(::Attr::Var::Ub, t_var, t_subproblem_id, t_value);

}

void Optimizers::DantzigWolfeDecomposition::apply_subproblem_bound_on_master(const Req<double, Var>& t_attr, const Var &t_var, unsigned int t_subproblem_id, double t_value) {

    auto& subproblem = m_subproblems[t_subproblem_id];

    auto& applied_bounds = t_attr == ::Attr::Var::Lb ? m_lower_bound_constraints : m_upper_bound_constraints;
    const auto it = applied_bounds.find(t_var);

    if (it == applied_bounds.end()) { // Create a new constraint

        auto expanded = expand_subproblem_variable(t_var, t_subproblem_id);
        const int type = t_attr == ::Attr::Var::Lb ? GreaterOrEqual : LessOrEqual;

        Ctr bound_constraint(m_master->env(), Equal, 0);

        m_master->add(bound_constraint, TempCtr(::Row(expanded, t_value), type));
        subproblem.m_generation_pattern.linear().set(bound_constraint, !t_var);

        applied_bounds.emplace(t_var, bound_constraint);

        return;
    }

    const auto& current_rhs = m_master->get_ctr_row(it->second).rhs();

    if (!current_rhs.is_numerical()) {
        throw Exception("Unexpected RHS with non-numerical terms.");
    }

    if (equals(t_value, current_rhs.numerical(), ToleranceForIntegrality)) { // Remove existing constraint for it is not useful anymore

        subproblem.m_generation_pattern.linear().remove(it->second);
        m_master->remove(it->second);
        applied_bounds.erase(it);

        return;
    }

    m_master->set_ctr_rhs(it->second, t_value);

}

LinExpr<Var> Optimizers::DantzigWolfeDecomposition::expand_subproblem_variable(const Var &t_var, unsigned int t_subproblem_id) {

    LinExpr<Var> result;

    for (const auto& [alpha, generator] : m_subproblems[t_subproblem_id].m_present_generators) {
        result += generator.get(t_var) * alpha;
    }

    return result;
}

void Optimizers::DantzigWolfeDecomposition::update_var_lb(const Var &t_var) {

    const unsigned int subproblem_id = t_var.get(m_variable_flag);

    if (subproblem_id != MasterId) {
        set_subproblem_lower_bound(t_var, subproblem_id, parent().get_var_lb(t_var));
        return;
    }

    ColumnGeneration::update_var_lb(t_var);
}

void Optimizers::DantzigWolfeDecomposition::update_var_ub(const Var &t_var) {

    const unsigned int subproblem_id = t_var.get(m_variable_flag);

    if (subproblem_id != MasterId) {
        set_subproblem_upper_bound(t_var, subproblem_id, parent().get_var_ub(t_var));
        return;
    }

    ColumnGeneration::update_var_ub(t_var);
}

void Optimizers::DantzigWolfeDecomposition::set_objective(Expr<Var, Var> &&t_objective) {

    const unsigned int n_subproblems = m_subproblems.size();

    Expr<Var, Var> master_obj = std::move(t_objective.constant());
    std::vector<Constant> pricing_obj(n_subproblems);

    for (auto [var, coeff] : t_objective.linear()) {
        const unsigned int subproblem_id = var.get(m_variable_flag);
        if (subproblem_id == MasterId) {
            master_obj += coeff * var;
        } else {
            if (!coeff.is_numerical()) {
                throw Exception("Could not handle non-numerical objective coefficient as generation pattern.");
            }
            pricing_obj[subproblem_id] += coeff.numerical() * !var;
        }
    }

    m_master->set_obj(std::move(master_obj));

    for (unsigned int k = 0 ; k < n_subproblems ; ++k) {
        m_subproblems[k].update_generation_pattern_objective(std::move(pricing_obj[k]));
    }

}
