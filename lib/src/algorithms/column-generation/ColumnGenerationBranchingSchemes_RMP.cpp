//
// Created by henri on 19/09/22.
//
#include "../../../include/algorithms/column-generation/ColumnGenerationBranchingSchemes_RMP.h"
#include "../../../include/algorithms/column-generation/ColumnGenerationSP.h"
#include "../../../include/modeling/expressions/operations/operators.h"
#include "../../../include/modeling/models/Model.h"


void ColumnGenerationBranchingSchemes::RMP::set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSP &t_subproblem) {

    t_subproblem.remove_columns_violating_lower_bound(t_var, t_lb);
    set_bound_rmp(
            t_var,
            t_lb,
            m_lower_bound_constraints,
            [](LinExpr<Var>&& t_expr, double t_b) { return std::move(t_expr) >= t_b; },
            [&](const Var& t_var){ return t_subproblem.exact_solution_strategy().get(Attr::Var::Lb, t_var); },
            t_subproblem
    );


}

void ColumnGenerationBranchingSchemes::RMP::set_upper_bound(const Var &t_var, double t_ub, ColumnGenerationSP &t_subproblem) {

    t_subproblem.remove_columns_violating_upper_bound(t_var, t_ub);
    set_bound_rmp(
            t_var,
            t_ub,
            m_upper_bound_constraints,
            [](LinExpr<Var>&& t_expr, double t_b) { return std::move(t_expr) <= t_b; },
            [&](const Var& t_var){ return t_subproblem.exact_solution_strategy().get(Attr::Var::Ub, t_var); },
            t_subproblem
    );


}

void ColumnGenerationBranchingSchemes::RMP::set_bound_rmp(const Var& t_subproblem_variable,
                                                          double t_bound,
                                                          Map<Var, Ctr>& t_bound_constraints,
                                                          const std::function<TempCtr(LinExpr<Var>&&, double)>& t_ctr_builder,
                                                          const std::function<double(const Var&)>& t_get_bound,
                                                          ColumnGenerationSP& t_subproblem) {

    auto& rmp = t_subproblem.rmp_solution_strategy();

    const auto it = t_bound_constraints.find(t_subproblem_variable);

    if (it == t_bound_constraints.end()) {

        auto expanded = t_subproblem.expand(t_subproblem_variable);
        auto ctr = rmp.add_ctr(t_ctr_builder(std::move(expanded), t_bound));
        t_subproblem.add_linking_expr(ctr, t_subproblem_variable);
        t_bound_constraints.emplace(t_subproblem_variable, ctr);
        return;

    }

    if (equals(t_bound, t_get_bound(t_subproblem_variable), ToleranceForIntegrality)) {
        auto constraint_to_remove = it->second;
        t_subproblem.reset_linking_expr(it->second);
        t_bound_constraints.erase(it);
        rmp.remove(constraint_to_remove);
        return;
    }

    rmp.set(Attr::Ctr::Rhs, it->second, t_bound);

}

std::optional<Ctr> ColumnGenerationBranchingSchemes::RMP::contribute_to_add_constraint(TempCtr &t_temporary_constraint,
                                                                                       ColumnGenerationSP &t_subproblem) {

    for (const auto& [var, ctr] : t_temporary_constraint.row().linear()) {
        if (!t_subproblem.exact_solution_strategy().get(Attr::Var::Status, var)) {
            return {};
        }
    }

    t_subproblem.remove_columns_violating_constraint(t_temporary_constraint);

    auto& row = t_temporary_constraint.row();

    // TODO this may be re-written in a better way after refacto of Row, Expr and Column
    LinExpr original_space;

    for (const auto& [var, coefficient] : row.linear()) {
        if (t_subproblem.exact_solution_strategy().get(Attr::Var::Status, var)) {
            original_space += coefficient * var;
        }
    }

    row.linear().replace_if([&](const Var& t_var) -> std::optional<LinExpr<Var>> {
        if (t_subproblem.exact_solution_strategy().get(Attr::Var::Status, t_var)) {
            return t_subproblem.expand(t_var);
        }
        return {};
    });

    auto result = t_subproblem.rmp_solution_strategy().add_ctr(std::move(t_temporary_constraint));

    t_subproblem.add_linking_expr(result, original_space);

    idol_Log(Trace, ColumnGenerationBranchingSchemes::RMP, "Constraint " << result << " was added to RMP.")

    return { result };
}
