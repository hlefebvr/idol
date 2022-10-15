//
// Created by henri on 21/09/22.
//
#include "../../../include/algorithms/decomposition/Decomposition.h"

Decomposition::Decomposition() {
    set_callback_attribute<Attr::CutOff>([this](double t_cutoff) {
        m_generation_strategies.front()->set<Attr::CutOff>(t_cutoff);
    });
}

void Decomposition::execute () {

    if (!m_rmp_strategy) {
        throw Exception("No solution strategy was given for RMP.");
    }

    if (m_generation_strategies.empty()) {
        throw Exception("No generation strategy was given to decomposition strategy.");
    }

    m_generation_strategies.front()->solve();
}


Solution::Primal Decomposition::primal_solution() const {
    return m_generation_strategies.front()->primal_solution();
}


Solution::Dual Decomposition::dual_solution() const {
    return m_generation_strategies.front()->dual_solution();
}


void Decomposition::update_lb(const Var &t_var, double t_lb) {
    m_generation_strategies.front()->update_lb(t_var, t_lb);
}


void Decomposition::update_ub(const Var &t_var, double t_ub) {
    m_generation_strategies.front()->update_ub(t_var, t_ub);
}

void Decomposition::update_coefficient_rhs(const Ctr &t_ctr, double t_rhs) {
    m_generation_strategies.front()->update_coefficient_rhs(t_ctr, t_rhs);
}

Ctr Decomposition::add_row(TempCtr t_temporary_constraint) {
    return m_generation_strategies.front()->add_row(std::move(t_temporary_constraint));
}

void Decomposition::remove(const Ctr &t_constraint) {
    m_generation_strategies.front()->remove(t_constraint);
}
