//
// Created by henri on 15/09/22.
//
#include "algorithms/solution-strategies/column-generation/generators/ColumnGenerator.h"
#include "algorithms/solution-strategies/column-generation/subproblems/ColumnGenerationSubproblem.h"

ColumnGenerator::ColumnGenerator(const Model& t_rmp, const Model &t_subproblem) : BaseGenerator(t_rmp, t_subproblem) {

    Expr objective;
    for (const auto& [var, coeff] : t_subproblem.objective()) {
        objective += coeff * var;
    }
    set_constant(objective);

}

TempVar ColumnGenerator::create_column(const Solution::Primal &t_primal_solution) const {
    Column column;

    double objective = 0.;
    for (const auto& [var, coeff] : constant()) {
        objective += coeff.constant() * t_primal_solution.get(var);
    }
    column.set_constant(objective);

    for (const auto& [ctr, expr] : *this) {
        double coeff = expr.constant().constant();
        for (const auto& [var, value] : expr) {
            coeff += value.constant() * t_primal_solution.get(var);
        }
        column.set(ctr, coeff);
    }

    return { lb(), ub(), type(), std::move(column) };
}

Row ColumnGenerator::get_pricing_objective(const Solution::Dual &t_dual_solution) {
    Row result = t_dual_solution.status() == Optimal ? Row(constant(), 0.) : Row();
    for (auto [ctr, row] : *this) {
        row *= -t_dual_solution.get(ctr);
        result += row;
    }
    return result;
}

void ColumnGenerator::set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) {

    if (t_var.model_id() != subproblem().id()) { return; }

    set_lower_bound_sp(t_var, t_lb, t_subproblem);
}

void ColumnGenerator::set_upper_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) {

    if (t_var.model_id() != subproblem().id()) { return; }

    set_upper_bound_sp(t_var, t_lb, t_subproblem);

}

void ColumnGenerator::set_lower_bound_sp(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) {
    remove_columns_violating_lower_bound(t_var, t_lb, t_subproblem);
    t_subproblem.exact_solution_strategy().set_lower_bound(t_var, t_lb);
}

void ColumnGenerator::set_upper_bound_sp(const Var &t_var, double t_ub, ColumnGenerationSubProblem &t_subproblem) {
    remove_columns_violating_upper_bound(t_var, t_ub, t_subproblem);
    t_subproblem.exact_solution_strategy().set_upper_bound(t_var, t_ub);
}

void ColumnGenerator::remove_columns_violating_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) {

    t_subproblem.remove_column_if([&](const Var& t_column_variable, const auto& t_column_primal_solution){
        if (double value = t_column_primal_solution.get(t_var) ; value < t_lb + ToleranceForIntegrality) {
            EASY_LOG(Trace,
                     "generation-strategies",
                     "Column " << t_column_variable << " was removed by contradiction with required "
                               << "bound " << t_var << " >= " << t_lb << " (" << t_var << " = " << value << ").");
            return true;
        }
        return false;
    });

}

void ColumnGenerator::remove_columns_violating_upper_bound(const Var &t_var, double t_ub, ColumnGenerationSubProblem &t_subproblem) {

    t_subproblem.remove_column_if([&](const Var& t_column_variable, const auto& t_column_primal_solution){
        if (double value = t_column_primal_solution.get(t_var) ; value > t_ub - ToleranceForIntegrality) {
            EASY_LOG(Trace,
                     "generation-strategies",
                     "Column " << t_column_variable << " was removed by contradiction with required "
                               << "bound " << t_var << " <= " << t_ub << " (" << t_var << " = " << value << ").");
            return true;
        }
        return false;
    });

}

Solution::Primal ColumnGenerator::primal_solution(const ColumnGenerationSubProblem& t_subproblem, const Solution::Primal& t_rmp_primals) const {
    Solution::Primal result;
    for (const auto& [var, ptr_to_sol] : t_subproblem.currently_present_variables()) {
        result += t_rmp_primals.get(var) * (ptr_to_sol);
    }
    return result;
}
