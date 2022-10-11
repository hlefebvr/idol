//
// Created by henri on 14/09/22.
//
#include "algorithms/branch-and-bound/Nodes_Basic.h"
#include "algorithms/Algorithm.h"
#include <iostream>

Nodes::Basic::Basic(unsigned int t_id, const Basic &t_src)
    : Node(t_id),
      m_local_upper_bounds(t_src.m_local_upper_bounds),
      m_local_lower_bounds(t_src.m_local_lower_bounds) {

}

void Nodes::Basic::save_solution(const Algorithm& t_strategy) {
    m_primal_solutions = t_strategy.primal_solution();
}

Nodes::Basic *Nodes::Basic::create_child(unsigned int t_id) const {
    return new Nodes::Basic(t_id, *this);
}

void Nodes::Basic::set_local_lower_bound(const Var &t_var, double t_lb) {
    auto [it, success] = m_local_lower_bounds.emplace(t_var, t_lb);
    if (!success) {
        it->second = t_lb;
    }
}

void Nodes::Basic::set_local_upper_bound(const Var &t_var, double t_ub) {
    auto [it, success] = m_local_upper_bounds.emplace(t_var, t_ub);
    if (!success) {
        it->second = t_ub;
    }
}
