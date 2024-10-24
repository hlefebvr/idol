//
// Created by henri on 31.10.23.
//
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/DantzigWolfeInfeasibilityStrategy.h"

const idol::PrimalPoint &idol::DantzigWolfe::InfeasibilityStrategyFactory::Strategy::primal_solution() const {

    if (!m_primal_solution.has_value()) {
        throw Exception("Primal value not accessible.");
    }

    return m_primal_solution.value();
}
