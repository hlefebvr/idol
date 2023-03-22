//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODEUPDATORBYBOUND_H
#define IDOL_NODEUPDATORBYBOUND_H

#include "NodeUpdator.h"
#include "modeling/models/AbstractModel.h"

template<class NodeT>
class NodeUpdatorByBound : public NodeUpdator2<NodeT> {
    AbstractModel& m_model;

    Map<Var, double> m_lower_bounds;
    Map<Var, double> m_upper_bounds;

    void update_bounds(Map<Var, double>& t_currently_modified_variables_with_their_original_bound,
                              const Map<Var, double>& t_node_bounds,
                              const Req<double, Var>& t_bound_attr);

public:
    explicit NodeUpdatorByBound(AbstractModel& t_model) : m_model(t_model) {}

    void apply_local_updates(const NodeT& t_node) override {
        update_bounds(m_lower_bounds, t_node.local_lower_bounds(), Attr::Var::Lb);
        update_bounds(m_upper_bounds, t_node.local_upper_bounds(), Attr::Var::Ub);
    }

    void clear_local_updates() override {
        update_bounds(m_lower_bounds, Map<Var, double>(), Attr::Var::Lb);
        update_bounds(m_upper_bounds, Map<Var, double>(), Attr::Var::Ub);
    }
};

template<class NodeT>
void NodeUpdatorByBound<NodeT>::update_bounds(Map<Var, double> &t_currently_modified_variables_with_their_original_bound,
                                              const Map<Var, double> &t_node_bounds,
                                              const Req<double, Var> &t_bound_attr) {


    auto it = t_currently_modified_variables_with_their_original_bound.cbegin();
    const auto end = t_currently_modified_variables_with_their_original_bound.cend();

    while ( it != end ) {

        const auto [var, original_bound] = *it;
        auto result = t_node_bounds.find(var);

        if (result == t_node_bounds.end()) {
            m_model.set(t_bound_attr, var, original_bound);
            it = t_currently_modified_variables_with_their_original_bound.erase(it);
            continue;
        }

        if (!equals(result->second, m_model.get(t_bound_attr, var), ToleranceForIntegrality)) {
            m_model.set(t_bound_attr, var, result->second);
        }
        ++it;

    }

    for (const auto& [var, bound] : t_node_bounds) {
        const double original_bound = m_model.get(t_bound_attr, var);
        auto [_, success] = t_currently_modified_variables_with_their_original_bound.emplace(var, original_bound);
        if (success) {
            m_model.set(t_bound_attr, var, bound);
        }
    }


}

#endif //IDOL_NODEUPDATORBYBOUND_H
