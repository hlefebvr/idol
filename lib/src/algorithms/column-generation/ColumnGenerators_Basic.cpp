//
// Created by henri on 15/09/22.
//
#include "algorithms/column-generation/ColumnGenerators_Basic.h"
#include "algorithms/column-generation/ColumnGenerationSP.h"

ColumnGenerators::Basic::Basic(const Model& t_rmp, const Model &t_subproblem) : Generators::Basic<Ctr>(t_rmp, t_subproblem) {

    Expr objective;
    for (const auto& [var, coeff] : t_subproblem.objective()) {
        objective += coeff * var;
    }
    set_constant(objective);

}

TempVar ColumnGenerators::Basic::create_column(const Solution::Primal &t_primal_solution) const {
    Column column;

    double objective = 0.;
    for (const auto& [var, coeff] : constant()) {
        objective += coeff.numerical() * t_primal_solution.get(var);
    }
    column.set_objective_coefficient(objective);

    for (const auto& [ctr, expr] : *this) {
        double coeff = expr.rhs().numerical();
        for (const auto& [var, value] : expr.lhs()) {
            coeff += value.numerical() * t_primal_solution.get(var);
        }
        column.components().set(ctr, coeff);
    }

    return { lb(), ub(), type(), std::move(column) };
}

Row ColumnGenerators::Basic::get_pricing_objective(const Solution::Dual &t_dual_solution) {
    Row result = t_dual_solution.status() == Optimal ? Row(constant(), 0.) : Row();
    for (auto [ctr, row] : *this) {
        row *= -t_dual_solution.get(ctr);
        result += row;
    }
    return result;
}

bool ColumnGenerators::Basic::set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSP &t_subproblem) {

    if (t_var.model_id() != subproblem().id()) { return false; }

    set_lower_bound_sp(t_var, t_lb, t_subproblem);

    return true;
}

bool ColumnGenerators::Basic::set_upper_bound(const Var &t_var, double t_lb, ColumnGenerationSP &t_subproblem) {

    if (t_var.model_id() != subproblem().id()) { return false; }

    set_upper_bound_sp(t_var, t_lb, t_subproblem);

    return true;

}

void ColumnGenerators::Basic::set_lower_bound_sp(const Var &t_var, double t_lb, ColumnGenerationSP &t_subproblem) {
    remove_columns_violating_lower_bound(t_var, t_lb, t_subproblem);
    t_subproblem.exact_solution_strategy().set_lower_bound(t_var, t_lb);
}

void ColumnGenerators::Basic::set_upper_bound_sp(const Var &t_var, double t_ub, ColumnGenerationSP &t_subproblem) {
    remove_columns_violating_upper_bound(t_var, t_ub, t_subproblem);
    t_subproblem.exact_solution_strategy().set_upper_bound(t_var, t_ub);
}

void ColumnGenerators::Basic::remove_columns_violating_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSP &t_subproblem) {

    t_subproblem.remove_column_if([&](const Var& t_column_variable, const auto& t_column_primal_solution){
        if (double value = t_column_primal_solution.get(t_var) ; value < t_lb + ToleranceForIntegrality) {
            EASY_LOG(Trace,
                     "column-generation",
                     "Column " << t_column_variable << " was removed by contradiction with required "
                               << "bound " << t_var << " >= " << t_lb << " (" << t_var << " = " << value << ").");
            return true;
        }
        return false;
    });

}

void ColumnGenerators::Basic::remove_columns_violating_upper_bound(const Var &t_var, double t_ub, ColumnGenerationSP &t_subproblem) {

    t_subproblem.remove_column_if([&](const Var& t_column_variable, const auto& t_column_primal_solution){
        if (double value = t_column_primal_solution.get(t_var) ; value > t_ub - ToleranceForIntegrality) {
            EASY_LOG(Trace,
                     "column-generation",
                     "Column " << t_column_variable << " was removed by contradiction with required "
                               << "bound " << t_var << " <= " << t_ub << " (" << t_var << " = " << value << ").");
            return true;
        }
        return false;
    });

}

Solution::Primal ColumnGenerators::Basic::primal_solution(const ColumnGenerationSP& t_subproblem, const Solution::Primal& t_rmp_primals) const {
    Solution::Primal result;

    double sum_primals = 0.;

    for (const auto& [var, primal_solution] : t_subproblem.currently_present_variables()) {
        const double primal = t_rmp_primals.get(var);

        if (!equals(primal, 0., ToleranceForSparsity)) {
            result += primal * primal_solution;
            sum_primals += primal;
        }
    }

    if (!equals(sum_primals, 1., ToleranceForSparsity)) {
        result *= 1. / sum_primals;
    }

    return result;
}

std::optional<Ctr> ColumnGenerators::Basic::contribute_to_add_constraint(TempCtr &t_temporary_constraint, ColumnGenerationSP& t_subproblem) {

    for (const auto& [var, ctr] : t_temporary_constraint.row().lhs()) {
        if (var.model_id() != subproblem().id()) {
            return {};
        }
    }

    remove_columns_violating_constraint(t_temporary_constraint, t_subproblem);

    auto result = t_subproblem.exact_solution_strategy().add_constraint(std::move(t_temporary_constraint));

    EASY_LOG(Trace, "column-generation", "Constraint " << result << " was added to subproblem.")

    return { result };
}

void ColumnGenerators::Basic::remove_columns_violating_constraint(const TempCtr &t_ctr, ColumnGenerationSP &t_subproblem) {

    t_subproblem.remove_column_if([&](const Var& t_column_variable, const Solution::Primal& t_solution) {
        if (t_ctr.is_violated(t_solution)) {
            EASY_LOG(Trace,
                     "column-generation",
                     "Column " << t_column_variable << " was removed by contradiction with required "
                               << "constraint " << t_ctr << '.');
            return true;
        }
        return false;
    });

}

bool ColumnGenerators::Basic::update_constraint_rhs(const Ctr &t_ctr, double t_rhs, ColumnGenerationSP &t_subproblem) {

    if (t_ctr.model_id() != subproblem().id()) {
        return false;
    }

    t_subproblem.exact_solution_strategy().update_constraint_rhs(t_ctr, t_rhs);

    return true;
}

bool ColumnGenerators::Basic::remove_constraint(const Ctr &t_ctr, ColumnGenerationSP &t_subproblem) {
    if (t_ctr.model_id() != subproblem().id()) {
        return false;
    }

    t_subproblem.exact_solution_strategy().remove_constraint(t_ctr);

    return true;
}