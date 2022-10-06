//
// Created by henri on 14/09/22.
//
#include "algorithms/branch-and-bound/nodes/NodeByBound.h"
#include "algorithms/solution-strategies/Algorithm.h"
#include "solvers/Solver.h"
#include <iostream>

NodeByBound::NodeByBound(unsigned int t_id, const NodeByBound &t_src)
    : AbstractNode(t_id),
      m_local_upper_bounds(t_src.m_local_upper_bounds),
      m_local_lower_bounds(t_src.m_local_lower_bounds) {

}

void NodeByBound::save_solution(const Algorithm& t_strategy) {
    m_primal_solutions = t_strategy.primal_solution();
}

NodeByBound *NodeByBound::create_child(unsigned int t_id) const {
    return new NodeByBound(t_id, *this);
}

void NodeByBound::set_local_lower_bound(const Var &t_var, double t_lb) {
    auto [it, success] = m_local_lower_bounds.emplace(t_var, t_lb);
    if (!success) {
        it->second = t_lb;
    }
}

void NodeByBound::set_local_upper_bound(const Var &t_var, double t_ub) {
    auto [it, success] = m_local_upper_bounds.emplace(t_var, t_ub);
    if (!success) {
        it->second = t_ub;
    }
}
