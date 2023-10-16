//
// Created by henri on 16.10.23.
//
#include "optimizers/callbacks/SimpleRounding.h"
#include "modeling/models/Model.h"
#include "modeling/expressions/operations/operators.h"

void idol::Heuristics::SimpleRounding::Strategy::operator()(CallbackEvent t_event) {

    if (t_event != InvalidSolution) {
        return;
    }

    const auto& model = this->original_model();
    const auto& primal_solution = this->primal_solution();

    auto result = primal_solution;

    for (const auto& [var, value] : primal_solution) {

        if (is_integer(value, Tolerance::Integer) || model.get_var_type(var) == Continuous) {
            continue;
        }

        const auto& column = model.get_var_column(var);

        if (!column.quadratic().empty()) {
            throw Exception("SimpleRounding for quadratic problems is not implemented.");
        }

        std::optional<bool> is_trivially_up_roundable = 0;
        for (const auto& [ctr, coefficient] : column.linear()) {

            if (!coefficient.is_numerical()) {
                throw Exception("A coefficient which is non numerical was found when rounding.");
            }

            bool direction = coefficient.numerical() >= 0.;

            if (!is_trivially_up_roundable.has_value()) {
                is_trivially_up_roundable = direction;
                continue;
            }

            if (is_trivially_up_roundable != direction) {
                return;
            }

        }

        if (!is_trivially_up_roundable.has_value()) {
            continue;
        }

        if (is_trivially_up_roundable) {
            result.set(var, std::ceil(value));
        } else {
            result.set(var, std::floor(value));
        }

    }

    result.set_objective_value(evaluate(model.get_obj_expr(), result));

    this->submit_heuristic_solution(result);

}
