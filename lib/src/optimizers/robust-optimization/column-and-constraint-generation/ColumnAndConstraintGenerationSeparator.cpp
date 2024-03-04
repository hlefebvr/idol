//
// Created by henri on 09.02.24.
//
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/ColumnAndConstraintGenerationSeparator.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/modeling/models/Model.h"
#include "idol/modeling/objects/Versions.h"
#include "idol/modeling/expressions/operations/operators.h"

idol::Expr<idol::Var, idol::Var> idol::ColumnAndConstraintGenerationSeparator::fix_and_revert(const LinExpr<Var> &t_expr,
                                                                             const Optimizers::ColumnAndConstraintGeneration &t_parent,
                                                                             const Solution::Primal &t_upper_level_solution) {
    Expr<Var, Var> result;

    const auto& lower_level_variables = t_parent.lower_level_variables();

    for (const auto& [var, constant] : t_expr) {

        const bool is_upper_level_variable = var.get(lower_level_variables) == MasterId;

        for (const auto& [param, coefficient] : constant.linear()) {

            const auto& uncertain_parameter = param.as<Var>();

            if (is_upper_level_variable) {
                result += coefficient * uncertain_parameter * t_upper_level_solution.get(var);
                continue;
            }

            result += coefficient * uncertain_parameter * var;
        }

        if (constant.quadratic().size() > 0) {
            throw Exception("Quadratic terms in uncertainty not implemented.");
        }

        if (is_upper_level_variable) {
            result += constant.numerical() * t_upper_level_solution.get(var);
            continue;
        }

        result += constant.numerical() * var;

    }

    return result;
}

idol::Expr<idol::Var, idol::Var> idol::ColumnAndConstraintGenerationSeparator::revert(const idol::Constant &t_constant,
                                                                                      const idol::Optimizers::ColumnAndConstraintGeneration &t_parent) {

    if (t_constant.quadratic().size() > 0) {
        throw Exception("Quadratic terms in uncertainty not implemented.");
    }

    Expr<Var, Var> result = t_constant.numerical();

    for (const auto& [param, coefficient] : t_constant.linear()) {

        const auto& uncertain_parameter = param.as<Var>();

        result += coefficient * uncertain_parameter;
    }

    return result;
}

