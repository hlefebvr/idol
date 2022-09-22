//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_NODEUPDATORBYBOUND_H
#define OPTIMIZE_NODEUPDATORBYBOUND_H

#include "AbstractNodeUpdatorStrategy.h"
#include "containers/Map.h"
#include "modeling/variables/Variable.h"
#include "modeling/numericals.h"
#include "algorithms/solution-strategies/AbstractSolutionStrategy.h"
#include <list>
#include <functional>

class NodeUpdatorByBound {
public:
    template<class NodeT> class Strategy;
};

template<class NodeT>
class NodeUpdatorByBound::Strategy : public AbstractNodeUpdatorStrategyWithType<NodeT> {

    Map<Var, double> m_lower_bounds;
    Map<Var, double> m_upper_bounds;

    static void update_bounds(Map<Var, double>& t_currently_modified_variables_with_their_original_bound,
                              const Map<Var, double>& t_node_bounds,
                              const std::function<void(const Var&, double)>&  t_set_bound_function,
                              const std::function<double(const Var&)>& t_get_bound_function);

public:
    Strategy() = default;

    void apply_local_changes(const NodeT &t_node, AbstractSolutionStrategy &t_solution_strategy);
};

template<class NodeT>
void NodeUpdatorByBound::Strategy<NodeT>::update_bounds(
        Map<Var, double> &t_currently_modified_variables_with_their_original_bound,
        const Map<Var, double> &t_node_bounds, const std::function<void(const Var &, double)> &t_set_bound_function,
        const std::function<double(const Var &)> &t_get_bound_function) {


    auto it = t_currently_modified_variables_with_their_original_bound.cbegin();
    const auto end = t_currently_modified_variables_with_their_original_bound.cend();

    while ( it != end ) {

        const auto [var, original_bound] = *it;
        auto result = t_node_bounds.find(var);

        if (result == t_node_bounds.end()) {
            t_set_bound_function(var, original_bound);
            it = t_currently_modified_variables_with_their_original_bound.erase(it);
            continue;
        }

        if (!equals(result->second, t_get_bound_function(var), ToleranceForIntegrality)) {
            t_set_bound_function(var, result->second);
        }
        ++it;

    }

    for (const auto& [var, bound] : t_node_bounds) {
        auto [_, success] = t_currently_modified_variables_with_their_original_bound.emplace(var, t_get_bound_function(var));
        if (success) {
            t_set_bound_function(var, bound);
        }
    }


}

template<class NodeT>
void NodeUpdatorByBound::Strategy<NodeT>::apply_local_changes(const NodeT &t_node, AbstractSolutionStrategy &t_solution_strategy) {

    update_bounds(
            m_lower_bounds,
            t_node.m_local_lower_bounds,
            [&t_solution_strategy](const Var& t_var, double t_lb) { t_solution_strategy.set_lower_bound(t_var, t_lb); },
            [](const Var& t_var){ return t_var.lb(); }
    );

    update_bounds(
            m_upper_bounds,
            t_node.m_local_upper_bounds,
            [&t_solution_strategy](const Var& t_var, double t_ub) { t_solution_strategy.set_upper_bound(t_var, t_ub); },
            [](const Var& t_var){ return t_var.ub(); }
    );

}

#endif //OPTIMIZE_NODEUPDATORBYBOUND_H
