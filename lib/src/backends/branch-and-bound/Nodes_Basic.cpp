//
// Created by henri on 14/09/22.
//
#include "backends/branch-and-bound/Nodes_Basic.h"
#include "modeling/models/Model.h"
#include <iostream>

Nodes::Basic::Basic(unsigned int t_id, const Basic &t_src)
    : Node(t_id),
      m_level(t_src.m_level+1),
      m_local_upper_bounds(t_src.m_local_upper_bounds),
      m_local_lower_bounds(t_src.m_local_lower_bounds) {

}

void Nodes::Basic::save_solution(const Model& t_strategy) {
    m_primal_solutions = save(t_strategy, Attr::Solution::Primal);
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

void Nodes::Basic::set_solution(Solution::Primal &&t_solution) {
    m_primal_solutions = std::move(t_solution);
}

std::optional<double> Nodes::Basic::get_local_bound(const Var &t_var, const Map<Var, double> &t_local_bounds) {
    auto it = t_local_bounds.find(t_var);
    if (it == t_local_bounds.end()) {
        return {};
    }
    return it->second;
}
