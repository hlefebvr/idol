//
// Created by henri on 16/09/22.
//
#include "algorithms/branch-and-bound/NodeByBoundStrategy.h"
#include "algorithms/branch-and-bound/AbstractSolutionStrategy.h"


void NodeByBoundStrategy::update_bounds(Map<Var, double> &t_currently_modified_variables_with_their_original_bound,
                                        const Map<Var, double> &t_node_bounds,
                                        const std::function<void(const Var &, double)> &t_set_bound_function,
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

void NodeByBoundStrategy::apply_local_changes(const NodeByBound &t_node, AbstractSolutionStrategy &t_solution_strategy) {
    //old_apply_local_changes(t_node, t_solution_strategy);
    new_apply_local_changes(t_node, t_solution_strategy);
}

void
NodeByBoundStrategy::new_apply_local_changes(const NodeByBound &t_node, AbstractSolutionStrategy &t_solution_strategy) {
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

void
NodeByBoundStrategy::old_apply_local_changes(const NodeByBound &t_node, AbstractSolutionStrategy &t_solution_strategy) {
    reset_changes(t_solution_strategy);

    for (const auto& [var, lb] : t_node.local_lower_bounds()) {
        m_original_lower_bounds.emplace_back(var, var.lb());
        t_solution_strategy.set_lower_bound(var, lb);
    }

    for (const auto& [var, ub] : t_node.local_upper_bounds()) {
        m_original_upper_bounds.emplace_back(var, var.ub());
        t_solution_strategy.set_upper_bound(var, ub);
    }
}

void NodeByBoundStrategy::reset_changes(AbstractSolutionStrategy& t_solution_strategy) {
    for (auto it = m_original_lower_bounds.rbegin(), end = m_original_lower_bounds.rend() ; it != end ; ++it) {
        t_solution_strategy.set_lower_bound(it->first, it->second);
    }
    m_original_lower_bounds.clear();
    for (auto it = m_original_upper_bounds.rbegin(), end = m_original_upper_bounds.rend() ; it != end ; ++it) {
        t_solution_strategy.set_upper_bound(it->first, it->second);
    }
    m_original_upper_bounds.clear();
}
