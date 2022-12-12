//
// Created by henri on 21/09/22.
//
#include "../../../include/algorithms/decomposition/Decomposition.h"

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


void Decomposition::update_var_lb(const Var &t_var, double t_lb) {
    m_generation_strategies.front()->update_var_lb(t_var, t_lb);
}


void Decomposition::update_var_ub(const Var &t_var, double t_ub) {
    m_generation_strategies.front()->update_var_ub(t_var, t_ub);
}

void Decomposition::update_rhs_coeff(const Ctr &t_ctr, double t_rhs) {
    m_generation_strategies.front()->update_rhs_coeff(t_ctr, t_rhs);
}

Ctr Decomposition::add_ctr(TempCtr&& t_temporary_constraint) {
    return m_generation_strategies.front()->add_ctr(std::move(t_temporary_constraint));
}

void Decomposition::remove(const Ctr &t_constraint) {
    m_generation_strategies.front()->remove(t_constraint);
}

AttributeManager &Decomposition::attribute_delegate(const Attribute &t_attribute) {
    return *m_generation_strategies.front();
}

AttributeManager &Decomposition::attribute_delegate(const Attribute &t_attribute, const Var &t_object) {
    return *m_generation_strategies.front();
}

AttributeManager &Decomposition::attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) {
    return *m_generation_strategies.front();
}

void Decomposition::set(const Parameter<double> &t_param, double t_value) {
    m_generation_strategies.front()->set(t_param, t_value);
}

void Decomposition::set(const Parameter<bool> &t_param, bool t_value) {
    m_generation_strategies.front()->set(t_param, t_value);
}

void Decomposition::set(const Parameter<int> &t_param, int t_value) {
    m_generation_strategies.front()->set(t_param, t_value);
}

double Decomposition::get(const Parameter<double> &t_param) const {
    return m_generation_strategies.front()->get(t_param);
}

bool Decomposition::get(const Parameter<bool> &t_param) const {
    return m_generation_strategies.front()->get(t_param);
}

int Decomposition::get(const Parameter<int> &t_param) const {
    return m_generation_strategies.front()->get(t_param);
}
