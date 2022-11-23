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

double Decomposition::get_lb(const Var &t_var) const {
    if (m_rmp_strategy->has(t_var)) {
        return m_rmp_strategy->get_lb(t_var);
    }
    for (auto& ptr_to_generation_strategy : m_generation_strategies) {
        if (ptr_to_generation_strategy->has(t_var)) {
            return ptr_to_generation_strategy->get_lb(t_var);
        }
    }
    throw Exception("Variable not found.");
}

double Decomposition::get_ub(const Var &t_var) const {
    if (m_rmp_strategy->has(t_var)) {
        return m_rmp_strategy->get_ub(t_var);
    }
    for (auto& ptr_to_generation_strategy : m_generation_strategies) {
        if (ptr_to_generation_strategy->has(t_var)) {
            return ptr_to_generation_strategy->get_ub(t_var);
        }
    }
    throw Exception("Variable not found.");
}

VarType Decomposition::get_type(const Var &t_var) const {
    if (m_rmp_strategy->has(t_var)) {
        return m_rmp_strategy->get_type(t_var);
    }
    for (auto& ptr_to_generation_strategy : m_generation_strategies) {
        if (ptr_to_generation_strategy->has(t_var)) {
            return ptr_to_generation_strategy->get_type(t_var);
        }
    }
    throw Exception("Variable not found.");
}

const Column &Decomposition::get_column(const Var &t_var) const {
    if (m_rmp_strategy->has(t_var)) {
        return m_rmp_strategy->get_column(t_var);
    }
    for (auto& ptr_to_generation_strategy : m_generation_strategies) {
        if (ptr_to_generation_strategy->has(t_var)) {
            return ptr_to_generation_strategy->get_column(t_var);
        }
    }
    throw Exception("Variable not found.");
}

bool Decomposition::has(const Var &t_var) const {
    if (m_rmp_strategy->has(t_var)) {
        return true;
    }
    for (auto& ptr_to_generation_strategy : m_generation_strategies) {
        if (ptr_to_generation_strategy->has(t_var)) {
            return true;
        }
    }
    return false;
}

const Row &Decomposition::get_row(const Ctr &t_ctr) const {
    if (m_rmp_strategy->has(t_ctr)) {
        return m_rmp_strategy->get_row(t_ctr);
    }
    for (auto& ptr_to_generation_strategy : m_generation_strategies) {
        if (ptr_to_generation_strategy->has(t_ctr)) {
            return ptr_to_generation_strategy->get_row(t_ctr);
        }
    }
    throw Exception("Variable not found.");
}

CtrType Decomposition::get_type(const Ctr &t_ctr) const {
    if (m_rmp_strategy->has(t_ctr)) {
        return m_rmp_strategy->get_type(t_ctr);
    }
    for (auto& ptr_to_generation_strategy : m_generation_strategies) {
        if (ptr_to_generation_strategy->has(t_ctr)) {
            return ptr_to_generation_strategy->get_type(t_ctr);
        }
    }
    throw Exception("Variable not found.");
}

bool Decomposition::has(const Ctr &t_ctr) const {
    if (m_rmp_strategy->has(t_ctr)) {
        return true;
    }
    for (auto& ptr_to_generation_strategy : m_generation_strategies) {
        if (ptr_to_generation_strategy->has(t_ctr)) {
            return true;
        }
    }
    return false;
}
