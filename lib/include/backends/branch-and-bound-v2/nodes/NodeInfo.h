//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODEINFO_H
#define IDOL_NODEINFO_H

#include "modeling/models/AbstractModel.h"
#include "NodeUpdator.h"

class NodeInfo {
    Solution::Primal m_primal_solution;

    Map<Var, double> m_local_lower_bounds;
    Map<Var, double> m_local_upper_bounds;

    NodeInfo(const NodeInfo& t_parent)
        : m_local_lower_bounds(t_parent.m_local_lower_bounds),
          m_local_upper_bounds(t_parent.m_local_upper_bounds) {}
public:
    NodeInfo() = default;

    virtual ~NodeInfo() = default;

    [[nodiscard]] double objective_value() const { return m_primal_solution.objective_value(); }

    [[nodiscard]] int status() const { return m_primal_solution.status(); }

    [[nodiscard]] int reason() const { return m_primal_solution.reason(); }

    [[nodiscard]] const auto& primal_solution() const { return m_primal_solution; }

    [[nodiscard]] const auto& local_lower_bounds() const { return m_local_lower_bounds; }

    [[nodiscard]] const auto& local_upper_bounds() const { return m_local_upper_bounds; }

    void set_local_lower_bound(const Var& t_var, double t_lb);

    void set_local_upper_bound(const Var& t_var, double t_ub);

    void save(const AbstractModel& t_original_formulation, const AbstractModel& t_model) { m_primal_solution = ::save(t_original_formulation, Attr::Solution::Primal, t_model); }

    static NodeUpdator2<NodeInfo>* create_updator(AbstractModel& t_model);

    [[nodiscard]] NodeInfo* create_child() const { return new NodeInfo(*this); }
};

#endif //IDOL_NODEINFO_H
