//
// Created by henri on 19.09.24.
//

#include "idol/mixed-integer/optimizers/padm/PenaltyUpdates.h"

void idol::PenaltyUpdate::operator()(std::list<ADM::Formulation::CurrentPenalty> &t_current_penalties) {

    for (auto& penalty : t_current_penalties) {
        penalty.penalty = (*this)(penalty.penalty);
    }

}

void
idol::PenaltyUpdates::Multiplicative::operator()(std::list<ADM::Formulation::CurrentPenalty> &t_current_penalties) {

    if (!m_normalized) {
        PenaltyUpdate::operator()(t_current_penalties);
        return;
    }

    double max_violation = 0;
    for (const auto& penalty : t_current_penalties) {
        max_violation = std::max(max_violation, penalty.max_violation);
    }

    // scales the factor from 1 to m_factor
    for (auto& penalty : t_current_penalties) {
        const double factor = 1 + (m_factor[m_current_factor_index] - 1.) * penalty.max_violation / max_violation;
        penalty.penalty = factor * penalty.penalty;
    }

}

bool idol::PenaltyUpdates::Multiplicative::diversify() {
    m_current_factor_index = (m_current_factor_index + 1) % m_factor.size();
    return m_current_factor_index != 0;
}

std::ostream &idol::PenaltyUpdates::Multiplicative::describe(std::ostream &t_os) const {
    t_os << "Multiplicative(";
    for (unsigned int i = 0; i < m_factor.size(); ++i) {
        t_os << m_factor[i];
        if (i < m_factor.size() - 1) {
            t_os << ", ";
        }
    }
    t_os << ")";
    return t_os;
}

idol::PenaltyUpdates::Adaptive::Adaptive(const std::vector<PenaltyUpdate*>& t_penalty_updates) {

    if (t_penalty_updates.empty()) {
        throw Exception("The list of penalty updates is empty.");
    }

    m_penalty_updates.reserve(t_penalty_updates.size());
    for (auto& penalty_update : t_penalty_updates) {
        m_penalty_updates.emplace_back(penalty_update);
    }

}

idol::PenaltyUpdates::Adaptive::Adaptive(const idol::PenaltyUpdates::Adaptive &t_src) : PenaltyUpdate(t_src), m_current_penalty_update_index(t_src.m_current_penalty_update_index) {

    m_penalty_updates.reserve(t_src.m_penalty_updates.size());
    for (auto& penalty_update : t_src.m_penalty_updates) {
        m_penalty_updates.emplace_back(penalty_update->clone());
    }

}

bool idol::PenaltyUpdates::Adaptive::diversify() {
    m_current_penalty_update_index = (m_current_penalty_update_index + 1) % m_penalty_updates.size();
    return m_current_penalty_update_index != 0;
}

std::ostream &idol::PenaltyUpdates::Adaptive::describe(std::ostream &t_os) const {
    t_os << "Adaptive(";
    for (unsigned int i = 0; i < m_penalty_updates.size(); ++i) {
        m_penalty_updates[i]->describe(t_os);
        if (i < m_penalty_updates.size() - 1) {
            t_os << ", ";
        }
    }
    t_os << "), current index: " << m_current_penalty_update_index;
    return t_os;
}

std::ostream &idol::operator<<(std::ostream &t_os, const PenaltyUpdate &t_penalty_update) {
    return t_penalty_update.describe(t_os);
}
