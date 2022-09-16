//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_NODEBYBOUNDSTRATEGY_H
#define OPTIMIZE_NODEBYBOUNDSTRATEGY_H

#include "BaseNodeStrategy.h"
#include "NodeByBound.h"
#include <list>

class NodeByBoundStrategy : public BaseNodeStrategy<NodeByBound> {
    std::list<std::pair<Var, double>> m_original_lower_bounds;
    std::list<std::pair<Var, double>> m_original_upper_bounds;

    Map<Var, double> m_lower_bounds;
    Map<Var, double> m_upper_bounds;

    static void update_bounds(Map<Var, double>& t_currently_modified_variables_with_their_original_bound,
                       const Map<Var, double>& t_node_bounds,
                       const std::function<void(const Var&, double)>&  t_set_bound_function,
                       const std::function<double(const Var&)>& t_get_bound_function);

    void reset_changes(AbstractSolutionStrategy& t_solution_strategy);

    void old_apply_local_changes(const NodeByBound &t_node, AbstractSolutionStrategy &t_solution_strategy);
    void new_apply_local_changes(const NodeByBound &t_node, AbstractSolutionStrategy &t_solution_strategy);
protected:
    void apply_local_changes(const NodeByBound &t_node, AbstractSolutionStrategy &t_solution_strategy) override;
public:
    NodeByBoundStrategy() = default;
};

#endif //OPTIMIZE_NODEBYBOUNDSTRATEGY_H
