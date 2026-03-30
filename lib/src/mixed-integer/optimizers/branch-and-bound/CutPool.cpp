//
// Created by Henri on 30/03/2026.
//
#include <utility>

#include "idol/mixed-integer/optimizers/branch-and-bound/CutPool.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

bool idol::CutPool::add_existing_cut_to_relaxation(const Ctr& t_cut, Model& t_relaxation) {

    // We only add the cut if it is sufficiently different from the others already in the relaxation
    for (const auto& history : m_cuts_in_relaxation) {
        if (std::abs(cosine(t_relaxation.env(), history.cut, t_cut)) > .95) {
            return false;
        }
    }

    t_relaxation.add(t_cut);
    m_cuts_in_relaxation.emplace_back(t_cut);

    return true;
}

bool idol::CutPool::add_cut(const TempCtr& t_cut, Model& t_relaxation) {
    Ctr cut(t_relaxation.env(), t_cut);
    m_all_cuts.emplace_back(cut);

    for (auto it = m_all_cuts.begin(), end = m_all_cuts.end() ; it != end && m_all_cuts.size() > m_max_pool_size ; ) {

        if (t_relaxation.has(*it)) {
            ++it;
        } else {
            it = m_all_cuts.erase(it);
        }

    }

    return add_existing_cut_to_relaxation(cut, t_relaxation);
}

unsigned int idol::CutPool::recycle(const PrimalPoint& t_current_point, Model& t_relaxation) {

    unsigned int result = 0;

    const auto& env = t_relaxation.env();

    for (auto& history : m_cuts_in_relaxation) {
        const auto& version = env[history.cut];
        history.age++;
        history.n_active += equals(evaluate(version.lhs(), t_current_point), version.rhs(), Tolerance::Feasibility);
    }

    // Check if the current point violates a previously generated cut
    for (const auto& ctr : m_all_cuts) {

        if (t_relaxation.has(ctr)) {
            continue;
        }

        // Check effectiveness of the cut
        const auto& version = env[ctr];
        double activity = 0.;
        double norm = 0.;

        for (const auto& [var, coefficient] : version.lhs()) {
            norm += coefficient * coefficient;
            activity += coefficient * t_current_point.get(var);
        }
        norm = std::sqrt(norm);

        double effectiveness = (activity - version.rhs()) / norm;
        if (version.type() == GreaterOrEqual) {
            effectiveness *= -1.;
        }

        if (effectiveness < .3) {
            continue;
        }

        result += add_existing_cut_to_relaxation(ctr, t_relaxation);

    }

    return result;
}

void idol::CutPool::clean_up(Model& t_relaxation) {

    for (auto it = m_cuts_in_relaxation.begin(), end = m_cuts_in_relaxation.end(); it != end ; ) {

        if (it->age < m_min_age) {
            ++it;
            continue;
        }

        if ((double) it->n_active / it->age > m_min_activity_threshold) {
            ++it;
            continue;
        }

        // Remove the cut
        t_relaxation.remove(it->cut);
        it = m_cuts_in_relaxation.erase(it);

    }

}

double idol::CutPool::cosine(const Env& t_env, const Ctr& t_cut1, const Ctr& t_cut2) {

    double result = 0;

    const auto& expr1 = t_env[t_cut1].lhs();
    const auto& expr2 = t_env[t_cut2].lhs();

    const auto* small = &expr1;
    const auto* large = &expr2;

    // iterate over the smaller map
    if (expr1.size() > expr2.size()) {
        small = &expr2;
        large = &expr1;
    }

    for (const auto& [var, coeff] : *small) {
        result += coeff * large->get(var);
    }

    // norm of expr1
    double norm1 = 0;
    for (const auto& [var, coeff] : expr1) {
        norm1 += coeff * coeff;
    }
    norm1 = std::sqrt(norm1);

    // norm of expr2
    double norm2 = 0;
    for (const auto& [var, coeff] : expr2) {
        norm2 += coeff * coeff;
    }
    norm2 = std::sqrt(norm2);

    result /= (norm1 * norm2);

    return result;
}

idol::CutPool::CutHistory::CutHistory(Ctr  t_cut) : cut(std::move(t_cut)) {

}
