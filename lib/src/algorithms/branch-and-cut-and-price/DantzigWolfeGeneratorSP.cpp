//
// Created by henri on 20/09/22.
//

#include "algorithms/branch-and-cut-and-price/DantzigWolfeGeneratorSP.h"
#include "algorithms/branch-and-cut-and-price/ColumnGenerationSubproblem.h"

DantzigWolfeGeneratorSP::DantzigWolfeGeneratorSP(Model &t_rmp, const Model &t_subproblem)
        : ColumnGenerator(t_rmp, t_subproblem),
          m_convexificiation_constraint(t_rmp.add_constraint(Expr() == 1, "convex")) {

    for (const auto& ctr : t_rmp.constraints()) {
        if (ctr.rhs().is_numerical()) { continue; }
        for (const auto& [param, coefficient] : ctr.rhs()) {
            if (param.variable().model_id() == t_subproblem.id()) {

                auto [it, success] = m_values.emplace(ctr, Row(-coefficient * param.variable(), 0.));
                if (!success) {
                    it->second += Row(-coefficient * param.variable(), 0.);
                }

            }
        }
    }

}

TempVar DantzigWolfeGeneratorSP::create_column(const Solution::Primal &t_primal_solution) const {
    auto result = ColumnGenerator::create_column(t_primal_solution);
    result.column().set(m_convexificiation_constraint, m_convexificiation_constraint.rhs().constant());
    return result;
}

Row DantzigWolfeGeneratorSP::get_pricing_objective(const Solution::Dual &t_dual_solution) {
    auto result = ColumnGenerator::get_pricing_objective(t_dual_solution);
    result.set_constant(result.constant() + -m_convexificiation_constraint.rhs().constant() * t_dual_solution.get(m_convexificiation_constraint));
    return result;
}

AbstractColumnGenerator *DantzigWolfeGeneratorSP::clone() const {
    return new DantzigWolfeGeneratorSP(*this);
}

void
DantzigWolfeGeneratorSP::set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) {

    set_lower_bound_sp(t_var, t_lb, t_subproblem);
}

void
DantzigWolfeGeneratorSP::set_upper_bound(const Var &t_var, double t_ub, ColumnGenerationSubProblem &t_subproblem) {

    set_upper_bound_sp(t_var, t_ub, t_subproblem);
}

Expr DantzigWolfeGeneratorSP::expand(const Var &t_subproblem_variable, const ColumnGenerationSubProblem& t_subproblem) {
    Expr result;
    for (const auto& [var, column] : t_subproblem.currently_present_variables()) {
        result += column.get(t_subproblem_variable) * var;
    }
    return result;
}
