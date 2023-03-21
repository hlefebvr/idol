//
// Created by henri on 21/03/23.
//
#include "backends/branch-and-bound-v2/nodes/NodeInfo.h"
#include "backends/branch-and-bound-v2/nodes/NodeUpdatorByBound.h"

NodeUpdator<NodeInfo> *NodeInfo::create_updator(AbstractModel& t_model) {
    return new NodeUpdatorByBound<NodeInfo>(t_model);
}

void NodeInfo::set_local_lower_bound(const Var &t_var, double t_lb) {
    auto [it, success] = m_local_lower_bounds.emplace(t_var, t_lb);
    if (!success) {
        it->second = t_lb;
    }
}

void NodeInfo::set_local_upper_bound(const Var &t_var, double t_ub) {
    auto [it, success] = m_local_upper_bounds.emplace(t_var, t_ub);
    if (!success) {
        it->second = t_ub;
    }
}
