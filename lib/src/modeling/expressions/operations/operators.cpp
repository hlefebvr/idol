//
// Created by henri on 16.10.23.
//
#include "modeling/expressions/operations/operators.h"
#include "modeling/solutions/Solution.h"

double idol::evaluate(const Expr<Var, Var>& t_expr, const Solution::Primal& t_values) {

    const auto& as_numerical = [](const Constant& t_constant) {

        if (!t_constant.is_numerical()) {
            throw Exception("A non-numerical constant was found while evaluating expression.");
        }

        return t_constant.numerical();

    };

    double result = as_numerical(t_expr.constant());

    for (const auto& [var, constant] : t_expr.linear()) {
        result += as_numerical(constant) * t_values.get(var);
    }

    return result;

}
