//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_NODEUPDATORS_BYBOUNDVAR_H
#define OPTIMIZE_NODEUPDATORS_BYBOUNDVAR_H

#include "NodeUpdator.h"
#include "containers/Map.h"
#include "modeling/models/AbstractModel.h"
#include <list>
#include <functional>

namespace NodeUpdators {
    class ByBoundVar;
}

class NodeUpdators::ByBoundVar {
public:
    template<class NodeT> class Strategy;
};

template<class NodeT>
class NodeUpdators::ByBoundVar::Strategy : public NodeUpdatorWithNodeType<NodeT> {

    Map<Var, double> m_lower_bounds;
    Map<Var, double> m_upper_bounds;

    static void update_bounds(Map<Var, double>& t_currently_modified_variables_with_their_original_bound,
                              const Map<Var, double>& t_node_bounds,
                              const std::function<void(const Var&, double)>&  t_set_bound_function,
                              const std::function<double(const Var&)>& t_get_bound_function);

public:
    Strategy() = default;

    void apply_local_changes(const NodeT &t_node, AbstractModel &t_solution_strategy) override;

    void revert_local_changes(AbstractModel &t_model) override;
};

template<class NodeT>
void NodeUpdators::ByBoundVar::Strategy<NodeT>::revert_local_changes(AbstractModel &t_model) {

    Map<Var, double> empty;

    update_bounds(
            m_lower_bounds,
            empty,
            [&](const Var& t_var, double t_lb) { t_model.set(Attr::Var::Lb, t_var, t_lb); },
            [&](const Var& t_var){ return t_model.get(Attr::Var::Lb, t_var); }
    );

    update_bounds(
            m_upper_bounds,
            empty,
            [&](const Var& t_var, double t_ub) { t_model.set(Attr::Var::Ub, t_var, t_ub); },
            [&](const Var& t_var){ return t_model.get(Attr::Var::Ub, t_var); }
    );

    m_lower_bounds.clear();
    m_upper_bounds.clear();

}

template<class NodeT>
void NodeUpdators::ByBoundVar::Strategy<NodeT>::update_bounds(
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
        const double org_bound = t_get_bound_function(var);
        auto [_, success] = t_currently_modified_variables_with_their_original_bound.emplace(var, org_bound);
        if (success) {
            t_set_bound_function(var, bound);
        }
    }


}

template<class NodeT>
void NodeUpdators::ByBoundVar::Strategy<NodeT>::apply_local_changes(const NodeT &t_node, AbstractModel &t_solution_strategy) {

    std::cout << "Node " << t_node.id() << std::endl;

    for (const auto& [var, lb] : m_lower_bounds) {
        std::cout << var << " >= " << lb << std::endl;
    }

    for (const auto& [var, ub] : m_upper_bounds) {
        std::cout << var << " <= " << ub << std::endl;
    }

    std::cout << "--" << std::endl;

    update_bounds(
            m_lower_bounds,
            t_node.local_lower_bounds(),
            [&](const Var& t_var, double t_lb) { t_solution_strategy.set(Attr::Var::Lb, t_var, t_lb); },
            [&](const Var& t_var){ return t_solution_strategy.get(Attr::Var::Lb, t_var); }
    );

    update_bounds(
            m_upper_bounds,
            t_node.local_upper_bounds(),
            [&](const Var& t_var, double t_ub) { t_solution_strategy.set(Attr::Var::Ub, t_var, t_ub); },
            [&](const Var& t_var){ return t_solution_strategy.get(Attr::Var::Ub, t_var); }
    );

}

#endif //OPTIMIZE_NODEUPDATORS_BYBOUNDVAR_H
