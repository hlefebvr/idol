//
// Created by henri on 22/09/22.
//

#ifndef OPTIMIZE_NODEUPDATORBYBOUNDCTR_H
#define OPTIMIZE_NODEUPDATORBYBOUNDCTR_H


#include "AbstractNodeUpdatorStrategy.h"
#include "modeling/variables/Variable.h"
#include "modeling/constraints/Constraint.h"
#include "modeling/constraints/TempCtr.h"
#include "containers/Map.h"
#include "modeling/numericals.h"
#include "algorithms/solution-strategies/AbstractSolutionStrategy.h"
#include <functional>

class NodeUpdatorByBoundCtr {
public:
    template<class NodeT> class Strategy;
};

template<class NodeT>
class NodeUpdatorByBoundCtr::Strategy : public AbstractNodeUpdatorStrategyWithType<NodeT> {

    Map<Var, Ctr> m_lower_bounds;
    Map<Var, Ctr> m_upper_bounds;

    static void update_bounds(Map<Var, Ctr>& t_current_constraints,
                              const Map<Var, double>& t_node_bounds,
                              const std::function<TempCtr(const Var&, double)>& t_create_temp_ctr_function,
                              AbstractSolutionStrategy& t_solution_strategy);
public:
    void apply_local_changes(const NodeT &t_node, AbstractSolutionStrategy &t_solution_strategy) override {

        update_bounds(m_lower_bounds,
                      t_node.m_local_lower_bounds,
                      [](const Var& t_x, double t_lb){ return t_x >= t_lb; },
                      t_solution_strategy);

        update_bounds(m_upper_bounds,
                      t_node.m_local_upper_bounds,
                      [](const Var& t_x, double t_ub){ return t_x <= t_ub; },
                      t_solution_strategy);

    }
};

template<class NodeT>
void NodeUpdatorByBoundCtr::Strategy<NodeT>::update_bounds(Map<Var, Ctr>& t_current_constraints,
                                                           const Map<Var, double>& t_node_bounds,
                                                           const std::function<TempCtr(const Var&, double)>& t_create_temp_ctr_function,
                                                           AbstractSolutionStrategy& t_solution_strategy) {


    auto it = t_current_constraints.cbegin();
    const auto end = t_current_constraints.cend();

    while ( it != end ) {

        const auto [var, constraint] = *it;
        auto result = t_node_bounds.find(var);

        if (result == t_node_bounds.end()) {
            t_solution_strategy.remove_constraint(constraint);
            it = t_current_constraints.erase(it);
            continue;
        }

        if (!equals(result->second, constraint.rhs().constant(), ToleranceForIntegrality)) {
            t_solution_strategy.update_constraint_rhs(constraint, result->second);
        }
        ++it;

    }

    for (const auto& [var, bound] : t_node_bounds) {
        auto result = t_current_constraints.find(var);
        if (result == t_current_constraints.end()) {
            auto constraint = t_solution_strategy.add_constraint(t_create_temp_ctr_function(var, bound));
            t_current_constraints.template emplace(var, constraint);
        }
    }


}


#endif //OPTIMIZE_NODEUPDATORBYBOUNDCTR_H
