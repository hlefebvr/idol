//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODEUPDATORBYBOUND_H
#define IDOL_NODEUPDATORBYBOUND_H

#include "NodeUpdator.h"
#include "modeling/models/Model.h"

template<class NodeT>
class NodeUpdatorByBound : public NodeUpdator<NodeT> {
    Model& m_model;

    Map<Var, double> m_lower_bounds;
    Map<Var, double> m_upper_bounds;

    void update_bounds(Map<Var, double>& t_currently_modified_variables_with_their_original_bound,
                              const Map<Var, double>& t_node_bounds,
                              bool t_is_lb);

public:
    explicit NodeUpdatorByBound(Model& t_model) : m_model(t_model) {}

    void apply_local_updates(const NodeT& t_node) override {
        update_bounds(m_lower_bounds, t_node.local_lower_bounds(), true);
        update_bounds(m_upper_bounds, t_node.local_upper_bounds(), false);
    }

    void clear_local_updates() override {
        update_bounds(m_lower_bounds, Map<Var, double>(), true);
        update_bounds(m_upper_bounds, Map<Var, double>(), false);
    }
};

template<class NodeT>
void NodeUpdatorByBound<NodeT>::update_bounds(Map<Var, double> &t_currently_modified_variables_with_their_original_bound,
                                              const Map<Var, double> &t_node_bounds,
                                              bool t_is_lb) {


    auto it = t_currently_modified_variables_with_their_original_bound.cbegin();
    const auto end = t_currently_modified_variables_with_their_original_bound.cend();

    while ( it != end ) {

        const auto [var, original_bound] = *it;
        auto result = t_node_bounds.find(var);

        if (result == t_node_bounds.end()) {
            t_is_lb ? m_model.set(Attr::Var::Lb, var, original_bound) : m_model.set(Attr::Var::Ub, var, original_bound);
            it = t_currently_modified_variables_with_their_original_bound.erase(it);
            continue;
        }

        const double bound = t_is_lb ? m_model.get_var_lb(var) : m_model.get_var_ub(var);
        if (!equals(result->second, bound, ToleranceForIntegrality)) {
            t_is_lb ? m_model.set(Attr::Var::Lb, var, result->second) : m_model.set(Attr::Var::Ub, var, result->second);
        }
        ++it;

    }

    for (const auto& [var, bound] : t_node_bounds) {
        const double original_bound = t_is_lb ? m_model.get_var_lb(var) : m_model.get_var_ub(var);
        auto [_, success] = t_currently_modified_variables_with_their_original_bound.emplace(var, original_bound);
        if (success) {
            t_is_lb ? m_model.set(Attr::Var::Lb, var, bound) : m_model.set(Attr::Var::Ub, var, bound);
        }
    }


}

#endif //IDOL_NODEUPDATORBYBOUND_H
