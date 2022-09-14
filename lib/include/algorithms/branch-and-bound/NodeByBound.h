//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_NODEBYBOUND_H
#define OPTIMIZE_NODEBYBOUND_H

#include "Node.h"
#include "../../solvers/solutions/Solution.h"
#include "../../containers/Map.h"
#include "../../modeling/variables/Variable.h"

class NodeByBound : public Node {
    Solution::Primal m_primal_solutions;
    Map<Var, double> m_local_lower_bounds;
    Map<Var, double> m_local_upper_bounds;

    NodeByBound(unsigned int t_id, const NodeByBound& t_src);
public:
    explicit NodeByBound(unsigned int t_id) : Node(t_id) {}

    void apply_local_changes(AbstractSolutionStrategy &t_strategy) override;
    void save_solution(const AbstractSolutionStrategy& t_strategy) override;

    SolutionStatus status() const override { return m_primal_solutions.status(); }
    double objective_value() const override { return m_primal_solutions.objective_value(); }
    const Solution::Primal& primal_solution() const override { return m_primal_solutions; }
    const Solution::Dual& dual_solution() const override { throw std::runtime_error("Not available."); };

    Node *create_child(unsigned int t_id) const override;
    void set_local_lower_bound(const Var &t_var, double t_lb) override;
    void set_local_upper_bound(const Var &t_var, double t_ub) override;
};


#endif //OPTIMIZE_NODEBYBOUND_H
