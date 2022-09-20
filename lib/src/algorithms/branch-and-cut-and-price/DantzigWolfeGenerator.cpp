//
// Created by henri on 19/09/22.
//
#include "algorithms/branch-and-cut-and-price/DantzigWolfeGenerator.h"
#include "algorithms/branch-and-cut-and-price/ColumnGenerationSubproblem.h"

DantzigWolfeGenerator::DantzigWolfeGenerator(const Model &t_rmp, const Model &t_subproblem, const Ctr& t_convexification_constraint)
    : ColumnGenerator(t_rmp, t_subproblem),
      m_convexificiation_constraint(t_convexification_constraint) {

}

void DantzigWolfeGenerator::set(const Var &t_rmp_var, const Var &t_sp_var) {
    if (!t_rmp_var.is_virtual()) {
        throw std::runtime_error("Only virtual variables can be added to a Dantzig-Wolfe column generator.");
    }

    if (auto [it, success] = m_rmp_to_subproblem_variables.emplace(t_rmp_var, t_sp_var) ; !success) {
        throw std::runtime_error("Trying to insert twice the same RMP variable.");
    }

    for (const auto& ctr : rmp().constraints()) {
        if (const auto& coefficient = ctr.get(t_rmp_var) ; !coefficient.is_zero()) {
            auto [it, success] = m_values.emplace(ctr, Row(coefficient * t_sp_var, 0.));
            if (!success) {
                it->second += Row(coefficient * t_sp_var, 0.);
            }
        }
    }

}

TempVar DantzigWolfeGenerator::create_column(const Solution::Primal &t_primal_solution) const {
    auto result = ColumnGenerator::create_column(t_primal_solution);
    result.column().set(m_convexificiation_constraint, 1.);
    return result;
}

Row DantzigWolfeGenerator::get_pricing_objective(const Solution::Dual &t_dual_solution) {
    auto result = ColumnGenerator::get_pricing_objective(t_dual_solution);
    result.set_constant(result.constant() + -m_convexificiation_constraint.rhs().constant() * t_dual_solution.get(m_convexificiation_constraint));
    return result;
}

AbstractColumnGenerator *DantzigWolfeGenerator::clone() const {
    return new DantzigWolfeGenerator(*this);
}

void
DantzigWolfeGenerator::set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) {

    if (t_var.model_id() == rmp().id()) {
        set_lower_bound_rmp(t_var, t_lb, t_subproblem);
        return;
    }

    if (t_var.model_id() == subproblem().id()) {
        set_lower_bound_sp(t_var, t_lb, t_subproblem);
        return;
    }
}

void
DantzigWolfeGenerator::set_upper_bound(const Var &t_var, double t_ub, ColumnGenerationSubProblem &t_subproblem) {

    if (t_var.model_id() == rmp().id()) {
        set_upper_bound_rmp(t_var, t_ub, t_subproblem);
        return;
    }

    if (t_var.model_id() == subproblem().id()) {
        set_upper_bound_sp(t_var, t_ub, t_subproblem);
        return;
    }
}

void DantzigWolfeGenerator::set_bound_rmp(const Var& t_subproblem_variable,
                                          double t_bound,
                                          Map<Var, Ctr>& t_bound_constraints,
                                          const std::function<TempCtr(Expr&&, double)>& t_ctr_builder,
                                          const std::function<double(const Var&)>& t_get_bound,
                                          ColumnGenerationSubProblem& t_subproblem) {

    auto& rmp = t_subproblem.rmp_solution_strategy();

    const auto it = t_bound_constraints.find(t_subproblem_variable);

    if (it == t_bound_constraints.end()) {

        Ctr ctr = rmp.add_constraint(t_ctr_builder(expand(t_subproblem_variable, t_subproblem), t_bound));
        ColumnGenerator::set(ctr, t_subproblem_variable);
        t_bound_constraints.emplace(t_subproblem_variable, ctr);
        return;

    }

    if (equals(t_bound, t_get_bound(t_subproblem_variable), ToleranceForIntegrality)) {
        auto constraint_to_remove = it->second;
        m_values.erase(it->second);
        t_bound_constraints.erase(it);
        rmp.remove_constraint(constraint_to_remove);
        return;
    }

    rmp.update_constraint_rhs(it->second, t_bound);

}

void DantzigWolfeGenerator::set_lower_bound_rmp(const Var &t_rmp_variable, double t_lb, ColumnGenerationSubProblem &t_subproblem) {

    auto it = m_rmp_to_subproblem_variables.find(t_rmp_variable);
    if (it == m_rmp_to_subproblem_variables.end()) {
        return;
    }

    set_bound_rmp(
            it->second,
            t_lb,
            m_lower_bound_constraints,
            [](Expr&& t_expr, double t_b) { return std::move(t_expr) >= t_b; },
            [](const Var& t_var){ return t_var.lb(); },
            t_subproblem
    );

}

void DantzigWolfeGenerator::set_upper_bound_rmp(const Var &t_rmp_variable, double t_ub, ColumnGenerationSubProblem &t_subproblem) {

    auto it = m_rmp_to_subproblem_variables.find(t_rmp_variable);
    if (it == m_rmp_to_subproblem_variables.end()) {
        return;
    }

    set_bound_rmp(
            it->second,
            t_ub,
            m_upper_bound_constraints,
            [](Expr&& t_expr, double t_b) { return std::move(t_expr) <= t_b; },
            [](const Var& t_var){ return t_var.ub(); },
            t_subproblem
    );

}

Solution::Primal DantzigWolfeGenerator::primal_solution(const ColumnGenerationSubProblem &t_subproblem,
                                                        const Solution::Primal &t_rmp_primals) const {
    auto result = ColumnGenerator::primal_solution(t_subproblem, t_rmp_primals);
    for (const auto& [rmp_var, sp_var] : m_rmp_to_subproblem_variables) {
        result.set(rmp_var, result.get(sp_var));
    }
    return result;
}

Var DantzigWolfeGenerator::get_subproblem_variable(const Var &t_rmp_variable) const {
    auto it = m_rmp_to_subproblem_variables.find(t_rmp_variable);
    if (it == m_rmp_to_subproblem_variables.end()) {
        throw std::runtime_error("Requested RMP variable could not be found.");
    }
    return it->second;
}

Expr DantzigWolfeGenerator::expand(const Var &t_subproblem_variable, const ColumnGenerationSubProblem& t_subproblem) {
    Expr result;
    for (const auto& [var, column] : t_subproblem.currently_present_variables()) {
        result += column.get(t_subproblem_variable) * var;
    }
    return result;
}
