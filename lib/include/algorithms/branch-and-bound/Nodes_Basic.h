//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_NODES_BASIC_H
#define OPTIMIZE_NODES_BASIC_H

#include "Node.h"
#include "../../modeling/solutions/Solution.h"
#include "../../containers/Map.h"
#include "../../modeling/variables/Variable.h"

namespace Nodes {
    class Basic;
}

class Nodes::Basic : public Node {
public: // TODO: remove this
    Solution::Primal m_primal_solutions;
    Map<Var, double> m_local_lower_bounds;
    Map<Var, double> m_local_upper_bounds;

    Basic(unsigned int t_id, const Basic& t_src);
public:
    explicit Basic(unsigned int t_id) : Node(t_id) {}

    void save_solution(const Algorithm& t_strategy) override;

    using LocalBounds = ConstIteratorForward<Map<Var, double>>;

    LocalBounds local_lower_bounds() const { return LocalBounds(m_local_lower_bounds); }
    LocalBounds local_upper_bounds() const { return LocalBounds(m_local_upper_bounds); }

    SolutionStatus status() const override { return m_primal_solutions.status(); }
    double objective_value() const override { return m_primal_solutions.objective_value(); }
    const Solution::Primal& primal_solution() const override { return m_primal_solutions; }
    const Solution::Dual& dual_solution() const override { throw Exception("Not available."); };

    Basic *create_child(unsigned int t_id) const override;
    void set_local_lower_bound(const Var &t_var, double t_lb) override;
    void set_local_upper_bound(const Var &t_var, double t_ub) override;
};


#endif //OPTIMIZE_NODES_BASIC_H
