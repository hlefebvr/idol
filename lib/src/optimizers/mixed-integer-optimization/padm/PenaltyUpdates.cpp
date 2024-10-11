//
// Created by henri on 19.09.24.
//

#include "idol/optimizers/mixed-integer-optimization/padm/PenaltyUpdates.h"

void idol::PenaltyUpdate::operator()(std::list<ADM::Formulation::CurrentPenalty> &t_current_penalties) {

    for (auto& penalty : t_current_penalties) {
        penalty.penalty = (*this)(penalty.penalty);
    }

}

void
idol::PenaltyUpdates::Multiplicative::operator()(std::list<ADM::Formulation::CurrentPenalty> &t_current_penalties) {

    if (!m_normalized) {
        PenaltyUpdate::operator()(t_current_penalties);
    }

    double max_violation = 0;
    for (const auto& penalty : t_current_penalties) {
        max_violation = std::max(max_violation, penalty.max_violation);
    }

    // scales the factor from 1 to m_factor
    for (auto& penalty : t_current_penalties) {
        penalty.penalty = 1 + (m_factor - 1.) * penalty.max_violation / max_violation;
    }

}
