//
// Created by henri on 24/03/23.
//
#include "optimizers/dantzig-wolfe/DantzigWolfe.h"
#include "modeling/objects/Versions.h"
#include "modeling/models/BlockModel.h"
#include "modeling/expressions/operations/operators.h"

Backends::DantzigWolfe::DantzigWolfe(const Model& t_original_formulation,
                                     const Annotation<Ctr, unsigned int>& t_constraint_flag,
                                     const Annotation<Var, unsigned int>& t_variable_flag,
                                     Model *t_master_problem,
                                     const std::vector<Model *> &t_subproblems,
                                     std::vector<Column> t_generation_patterns)
     : Backends::ColumnGeneration(t_original_formulation, t_master_problem, t_subproblems, std::move(t_generation_patterns)),
       m_variable_flag(t_variable_flag),
       m_constraint_flag(t_constraint_flag) {

}

double Backends::DantzigWolfe::get(const Req<double, Var> &t_attr, const Var &t_var) const {

    const unsigned int subproblem_id = t_var.get(m_variable_flag);

    if (subproblem_id != MasterId) {
        if (t_attr == Attr::Solution::Primal) {
            return get_subproblem_primal_value(t_var, subproblem_id);
        } else {
            return m_subproblems[subproblem_id].m_model->get(t_attr, t_var);
        }
    }

    return ColumnGeneration::get(t_attr, t_var);
}

void Backends::DantzigWolfe::set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) {

    const unsigned int subproblem_id = t_var.get(m_variable_flag);

    if (subproblem_id != MasterId) {

        if (t_attr == Attr::Var::Lb) {
            set_subproblem_lower_bound(t_var, subproblem_id, t_value);
            return;
        }

        if (t_attr == Attr::Var::Ub) {
            set_subproblem_upper_bound(t_var, subproblem_id, t_value);
            return;
        }

        m_subproblems[subproblem_id].m_model->set(t_attr, t_var, t_value);
        return;

    }

    ColumnGeneration::set(t_attr, t_var, t_value);
}


double Backends::DantzigWolfe::get_subproblem_primal_value(const Var &t_var, unsigned int t_subproblem_id) const {

    double result = 0;
    for (const auto& [alpha, generator] : m_subproblems[t_subproblem_id].m_present_generators) {
        const double alpha_val = m_master->get(::Attr::Solution::Primal, alpha);
        if (alpha_val > 0) {
            result += alpha_val * generator.get(t_var);
        }
    }

    return result;
}

void Backends::DantzigWolfe::set_subproblem_lower_bound(const Var &t_var, unsigned int t_subproblem_id, double t_value) {

    auto& subproblem = m_subproblems[t_subproblem_id];

    subproblem.remove_column_if([&](const Var& t_object, const Solution::Primal& t_generator) {
        //return true;
        return t_generator.get(t_var) < t_value;
    });

    if (!parent().get(::Param::ColumnGeneration::BranchingOnMaster)) {
        subproblem.m_model->set(::Attr::Var::Lb, t_var, t_value);
        return;
    }

    apply_subproblem_bound_on_master(::Attr::Var::Lb, t_var, t_subproblem_id, t_value);

}

void Backends::DantzigWolfe::set_subproblem_upper_bound(const Var &t_var, unsigned int t_subproblem_id, double t_value) {

    auto& subproblem = m_subproblems[t_subproblem_id];

    subproblem.remove_column_if([&](const Var& t_object, const Solution::Primal& t_generator) {
        //return true;
        return t_generator.get(t_var) > t_value;
    });

    if (!parent().get(::Param::ColumnGeneration::BranchingOnMaster)) {
        subproblem.m_model->set(::Attr::Var::Ub, t_var, t_value);
        return;
    }

    apply_subproblem_bound_on_master(::Attr::Var::Ub, t_var, t_subproblem_id, t_value);

}

void Backends::DantzigWolfe::apply_subproblem_bound_on_master(const Req<double, Var>& t_attr, const Var &t_var, unsigned int t_subproblem_id, double t_value) {

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

    const auto& current_rhs = m_master->get(::Attr::Ctr::Rhs, it->second);

    if (!current_rhs.is_numerical()) {
        throw Exception("Unexpected RHS with non-numerical terms.");
    }

    if (equals(t_value, current_rhs.numerical(), ToleranceForIntegrality)) { // Remove existing constraint for it is not useful anymore

        subproblem.m_generation_pattern.linear().remove(it->second);
        m_master->remove(it->second);
        applied_bounds.erase(it);

        return;
    }

    m_master->set(::Attr::Ctr::Rhs, it->second, t_value);

}

LinExpr<Var> Backends::DantzigWolfe::expand_subproblem_variable(const Var &t_var, unsigned int t_subproblem_id) {

    LinExpr<Var> result;

    for (const auto& [alpha, generator] : m_subproblems[t_subproblem_id].m_present_generators) {
        result += generator.get(t_var) * alpha;
    }

    return result;
}

void Backends::DantzigWolfe::set(const Req<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_expr) {

    if (t_attr == Attr::Obj::Expr) {

        const unsigned int n_subproblems = m_subproblems.size();

        Expr<Var, Var> master_obj = std::move(t_expr.constant());
        std::vector<Constant> pricing_obj(n_subproblems);

        for (auto [var, coeff] : t_expr.linear()) {
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

        m_master->set(t_attr, std::move(master_obj));

        for (unsigned int k = 0 ; k < n_subproblems ; ++k) {
            m_subproblems[k].m_generation_pattern.set_obj(std::move(pricing_obj[k]));
        }

        return;
    }

    Base::set(t_attr, t_expr);
}

