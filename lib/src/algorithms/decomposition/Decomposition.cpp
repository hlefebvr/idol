//
// Created by henri on 21/09/22.
//
#include "../../../include/algorithms/decomposition/Decomposition.h"

void Decomposition::build() {

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


void Decomposition::execute () {
    m_generation_strategies.front()->solve();
}


Solution::Primal Decomposition::primal_solution() const {
    return m_generation_strategies.front()->primal_solution();
}


Solution::Dual Decomposition::dual_solution() const {
    return m_generation_strategies.front()->dual_solution();
}


void Decomposition::set_lower_bound(const Var &t_var, double t_lb) {
    m_generation_strategies.front()->set_lower_bound(t_var, t_lb);
}


void Decomposition::set_upper_bound(const Var &t_var, double t_ub) {
    m_generation_strategies.front()->set_upper_bound(t_var, t_ub);
}

void Decomposition::update_constraint_rhs(const Ctr &t_ctr, double t_rhs) {
    m_generation_strategies.front()->update_constraint_rhs(t_ctr, t_rhs);
}

Ctr Decomposition::add_constraint(TempCtr t_temporary_constraint) {
    return m_generation_strategies.front()->add_constraint(std::move(t_temporary_constraint));
}

void Decomposition::remove_constraint(const Ctr &t_constraint) {
    m_generation_strategies.front()->remove_constraint(t_constraint);
}
