//
// Created by henri on 16.10.23.
//
#include "idol/optimizers/mixed-integer-programming/callbacks/heuristics/SimpleRounding.h"
#include "idol/modeling//models/Model.h"
#include "idol/modeling//expressions/operations/operators.h"

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

        std::optional<bool> is_trivially_up_roundable; // The name assumes <= constraints
        for (const auto& [ctr, coefficient] : column.linear()) {

            if (!coefficient.is_numerical()) {
                throw Exception("A coefficient which is non numerical was found when rounding.");
            }

            const auto type = model.get_ctr_type(ctr);

            if (type == Equal) {
                return;
            }

            bool direction = (type == LessOrEqual && coefficient.numerical() <= 0.) || (type == GreaterOrEqual && coefficient.numerical() >= 0.);

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

        if (is_trivially_up_roundable.value()) {
            result.set(var, std::ceil(value));
        } else {
            result.set(var, std::floor(value));
        }

    }

    result.set_objective_value(evaluate(model.get_obj_expr(), result));

    this->submit_heuristic_solution(result);

}
