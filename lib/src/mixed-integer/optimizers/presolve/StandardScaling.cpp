//
// Created by Henri on 11/06/2026.
//
#include "idol/mixed-integer/optimizers/presolve/StandardScaling.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

bool idol::Presolvers::StandardScaling::execute(Model& t_model) {

    for (const auto& ctr : t_model.ctrs()) {
        const auto& row = t_model.get_ctr_row(ctr);
        const double rhs = t_model.get_ctr_rhs(ctr);
        double infinity_norm = 0;
        for (const auto& [var, coeff] : row) {
            infinity_norm = std::max(std::abs(coeff), infinity_norm);
        }
        infinity_norm = std::max(infinity_norm, std::abs(rhs));

        if (is_zero(infinity_norm, Tolerance::Sparsity)) {
            continue;
        }

        int e = 0;
        std::frexp(infinity_norm, &e);
        double closest_power_of_2 = std::ldexp(1.0, e - 1); // scale = 2^(e-1) or 2^e depending on your normalization choice
        if (closest_power_of_2 != 1.) {
            t_model.set_ctr_row(ctr, row / closest_power_of_2);
            t_model.set_ctr_rhs(ctr, rhs / closest_power_of_2);
            m_n_rescaled++;
        }
    }

    return false;
}

void idol::Presolvers::StandardScaling::log_after_termination() const {
    AbstractPresolver::log_after_termination();

    std::cout << "Standard scaling: " << m_n_rescaled;
}
