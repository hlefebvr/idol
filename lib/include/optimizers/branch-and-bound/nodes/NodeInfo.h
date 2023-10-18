//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODEINFO_H
#define IDOL_NODEINFO_H

#include "modeling/models/Model.h"
#include "NodeUpdator.h"

namespace idol {
    class NodeInfo;
}

class idol::NodeInfo {
    Solution::Primal m_primal_solution;

    Map<Var, double> m_local_lower_bounds;
    Map<Var, double> m_local_upper_bounds;

protected:
    NodeInfo(const NodeInfo& t_parent)
        : m_local_lower_bounds(t_parent.m_local_lower_bounds),
          m_local_upper_bounds(t_parent.m_local_upper_bounds) {}
public:
    NodeInfo() = default;

    virtual ~NodeInfo() = default;

    [[nodiscard]] SolutionStatus status() const { return m_primal_solution.status(); }

    [[nodiscard]] SolutionReason reason() const { return m_primal_solution.reason(); }

    [[nodiscard]] bool has_objective_value() const { return m_primal_solution.has_objective_value(); }

    [[nodiscard]] double objective_value() const { return m_primal_solution.objective_value(); }

    [[nodiscard]] const auto& primal_solution() const { return m_primal_solution; }

    [[nodiscard]] const auto& local_lower_bounds() const { return m_local_lower_bounds; }

    [[nodiscard]] const auto& local_upper_bounds() const { return m_local_upper_bounds; }

    void set_local_lower_bound(const Var& t_var, double t_lb);

    void set_local_upper_bound(const Var& t_var, double t_ub);

    virtual void save(const Model& t_original_formulation, const Model& t_model);

    void set_primal_solution(Solution::Primal t_primal_solution) { m_primal_solution = std::move(t_primal_solution); }

    static NodeUpdator<NodeInfo>* create_updator(Model& t_model);

    [[nodiscard]] virtual NodeInfo* create_child() const { return new NodeInfo(*this); }
};

#endif //IDOL_NODEINFO_H
