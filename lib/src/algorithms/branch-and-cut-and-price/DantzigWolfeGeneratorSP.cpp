//
// Created by henri on 20/09/22.
//

#include "algorithms/column-generation/generators/DantzigWolfe_SP_Strategy.h"
#include "algorithms/column-generation/subproblems/ColumnGenerationSubproblem.h"

DantzigWolfe_SP_Strategy::DantzigWolfe_SP_Strategy(Model &t_rmp, const Model &t_subproblem)
        : ColumnGenerator(t_rmp, t_subproblem),
          m_convexificiation_constraint(t_rmp.add_constraint(Expr() == 1, "convex")) {

    for (const auto& ctr : t_rmp.constraints()) {
        if (ctr.rhs().is_numerical()) { continue; }
        for (const auto& [param, coefficient] : ctr.rhs()) {
            if (param.variable().model_id() == t_subproblem.id()) {

                auto [it, success] = m_values.emplace(ctr, Row(-coefficient * param.variable(), 0.));
                if (!success) {
                    it->second.lhs() += -coefficient * param.variable();
                }

            }
        }
    }

}

TempVar DantzigWolfe_SP_Strategy::create_column(const Solution::Primal &t_primal_solution) const {
    auto result = ColumnGenerator::create_column(t_primal_solution);
    result.column().components().set(m_convexificiation_constraint, m_convexificiation_constraint.rhs().numerical());
    return result;
}

Row DantzigWolfe_SP_Strategy::get_pricing_objective(const Solution::Dual &t_dual_solution) {
    auto result = ColumnGenerator::get_pricing_objective(t_dual_solution);
    result.rhs() += -m_convexificiation_constraint.rhs().numerical() * t_dual_solution.get(m_convexificiation_constraint);
    return result;
}

Expr<Var> DantzigWolfe_SP_Strategy::expand(const Var &t_subproblem_variable, const ColumnGenerationSubproblem& t_subproblem) {
    Expr result;
    for (const auto& [var, column] : t_subproblem.currently_present_variables()) {
        result += column.get(t_subproblem_variable) * var;
    }
    return result;
}
