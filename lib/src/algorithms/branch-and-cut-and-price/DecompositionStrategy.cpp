//
// Created by henri on 21/09/22.
//
#include "algorithms/branch-and-cut-and-price/DecompositionStrategy.h"

void DecompositionStrategy::build() {

    if (!m_rmp_strategy) {
        throw Exception("No solution strategy was given for RMP.");
    }

    m_rmp_strategy->build();

    if (m_generation_strategies.empty()) {
        throw Exception("No generation strategy was given to decomposition strategy.");
    }

    for (auto& ptr_to_generator_strategy : m_generation_strategies) {
        ptr_to_generator_strategy->build();
    }
}


void DecompositionStrategy::solve() {
    m_generation_strategies.front()->solve();
}


Solution::Primal DecompositionStrategy::primal_solution() const {
    return m_generation_strategies.front()->primal_solution();
}


Solution::Dual DecompositionStrategy::dual_solution() const {
    return m_generation_strategies.front()->dual_solution();
}


void DecompositionStrategy::set_lower_bound(const Var &t_var, double t_lb) {
    for (auto& ptr_to_decomposition_strategy : m_generation_strategies) {
        ptr_to_decomposition_strategy->set_lower_bound(t_var, t_lb);
    }
}


void DecompositionStrategy::set_upper_bound(const Var &t_var, double t_ub) {
    for (auto& ptr_to_decomposition_strategy : m_generation_strategies) {
        ptr_to_decomposition_strategy->set_upper_bound(t_var, t_ub);
    }
}
