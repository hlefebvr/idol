//
// Created by Henri on 11/06/2026.
//
#include "idol/mixed-integer/optimizers/presolve/StandardScaling.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

bool idol::Presolvers::StandardScaling::execute(Model& t_model) {

    constexpr int max_scaling_exp = 15;

    for (const auto& ctr : t_model.ctrs()) {

        const auto& row = t_model.get_ctr_row(ctr);
        const double rhs = t_model.get_ctr_rhs(ctr);

        double infinity_norm = std::abs(rhs);
        for (const auto& [var, coeff] : row) {
            infinity_norm = std::max(infinity_norm, std::abs(coeff));
        }

        if (is_zero(infinity_norm, Tolerance::Sparsity)) {
            continue;
        }

        int e = 0;
        std::frexp(infinity_norm, &e);

        // Cap the scaling exponent while preserving a power-of-two scaling factor.
        e = std::clamp(e - 1, -max_scaling_exp, max_scaling_exp);

        const double scaling_factor = std::ldexp(1.0, e);

        if (scaling_factor != 1.0) {
            t_model.set_ctr_row(ctr, row / scaling_factor);
            t_model.set_ctr_rhs(ctr, rhs / scaling_factor);
            ++m_n_rescaled;
        }
    }

    return false;
}

void idol::Presolvers::StandardScaling::log_after_termination() const {
    AbstractPresolver::log_after_termination();

    std::cout << "Standard scaling: " << m_n_rescaled;
}
