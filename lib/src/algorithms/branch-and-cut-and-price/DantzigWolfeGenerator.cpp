//
// Created by henri on 19/09/22.
//
#include "algorithms/solution-strategies/column-generation/generators/DantzigWolfe_RMP_Strategy.h"
#include "algorithms/solution-strategies/column-generation/subproblems/ColumnGenerationSubproblem.h"

DantzigWolfe_RMP_Strategy::DantzigWolfe_RMP_Strategy(Model &t_rmp, const Model &t_subproblem) : DantzigWolfe_SP_Strategy(t_rmp, t_subproblem) {}


bool DantzigWolfe_RMP_Strategy::set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) {

    if (t_var.model_id() != subproblem().id()) { return false; }

    set_lower_bound_rmp(t_var, t_lb, t_subproblem);

    return true;

}

bool
DantzigWolfe_RMP_Strategy::set_upper_bound(const Var &t_var, double t_ub, ColumnGenerationSubProblem &t_subproblem) {

    if (t_var.model_id() != subproblem().id()) { return false; }

    set_upper_bound_rmp(t_var, t_ub, t_subproblem);

    return true;

}

void DantzigWolfe_RMP_Strategy::set_bound_rmp(const Var& t_subproblem_variable,
                                              double t_bound,
                                              Map<Var, Ctr>& t_bound_constraints,
                                              const std::function<TempCtr(Expr<Var>&&, double)>& t_ctr_builder,
                                              const std::function<double(const Var&)>& t_get_bound,
                                              ColumnGenerationSubProblem& t_subproblem) {

    auto& rmp = t_subproblem.rmp_solution_strategy();

    const auto it = t_bound_constraints.find(t_subproblem_variable);

    if (it == t_bound_constraints.end()) {

        auto ctr = rmp.add_constraint(t_ctr_builder(expand(t_subproblem_variable, t_subproblem), t_bound));
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

void DantzigWolfe_RMP_Strategy::set_lower_bound_rmp(const Var &t_rmp_variable, double t_lb, ColumnGenerationSubProblem &t_subproblem) {

    set_bound_rmp(
            t_rmp_variable,
            t_lb,
            m_lower_bound_constraints,
            [](Expr<Var>&& t_expr, double t_b) { return std::move(t_expr) >= t_b; },
            [](const Var& t_var){ return t_var.lb(); },
            t_subproblem
    );

}

void DantzigWolfe_RMP_Strategy::set_upper_bound_rmp(const Var &t_rmp_variable, double t_ub, ColumnGenerationSubProblem &t_subproblem) {

    set_bound_rmp(
            t_rmp_variable,
            t_ub,
            m_upper_bound_constraints,
            [](Expr<Var>&& t_expr, double t_b) { return std::move(t_expr) <= t_b; },
            [](const Var& t_var){ return t_var.ub(); },
            t_subproblem
    );

}

std::optional<Ctr> DantzigWolfe_RMP_Strategy::contribute_to_add_constraint(TempCtr &t_temporary_constraint,
                                                                           ColumnGenerationSubProblem &t_subproblem) {

    for (const auto& [var, ctr] : t_temporary_constraint.row().lhs()) {
        if (var.model_id() != subproblem().id()) {
            return {};
        }
    }

    remove_columns_violating_constraint(t_temporary_constraint, t_subproblem);

    auto& row = t_temporary_constraint.row();
    const auto subproblem_id = subproblem().id();

    // TODO this may be re-written in a better way after refacto of Row, Expr and Column
    Expr original_space;

    for (const auto& [var, coefficient] : row.lhs()) {
        if (var.model_id() == subproblem_id) {
            original_space += coefficient * var;
        }
    }

    row.lhs().replace_if([&](const Var& t_var) -> std::optional<Expr<Var>> {
        if (t_var.model_id() == subproblem_id) {
            return expand(t_var, t_subproblem);
        }
        return {};
    });

    auto result = t_subproblem.rmp_solution_strategy().add_constraint(std::move(t_temporary_constraint));

    set(result, std::move(original_space));

    return result;
}

bool DantzigWolfe_RMP_Strategy::update_constraint_rhs(const Ctr &t_ctr, double t_rhs,
                                                      ColumnGenerationSubProblem &t_subproblem) {
    return DantzigWolfe_SP_Strategy::update_constraint_rhs(t_ctr, t_rhs, t_subproblem);
}

bool DantzigWolfe_RMP_Strategy::remove_constraint(const Ctr &t_ctr, ColumnGenerationSubProblem &t_subproblem) {
    if (t_ctr.model_id() == rmp().id()) {
        m_values.erase(t_ctr);
        return false;
    }
    return DantzigWolfe_SP_Strategy::remove_constraint(t_ctr, t_subproblem);
}
