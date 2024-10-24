//
// Created by henri on 18.10.23.
//

#ifndef IDOL_IMPL_PSEUDOCOST_H
#define IDOL_IMPL_PSEUDOCOST_H

#include <cmath>
#include "VariableBranching.h"
#include "NodeScoreFunction.h"

namespace idol::BranchingRules {
    template<class NodeInfoT> class PseudoCost;
}

template<class NodeInfoT>
class idol::BranchingRules::PseudoCost : public VariableBranching<NodeInfoT> {

    std::unique_ptr<NodeScoreFunction> m_scoring_function;

    struct PseudoCostData {
        double objective_gains_by_upper_boundings = 0;
        double objective_gains_by_lower_boundings = 0;
        unsigned int n_entries_for_upper_bounds = 0;
        unsigned int n_entries_for_lower_bounds = 0;

        PseudoCostData& operator+=(const PseudoCostData& t_rhs);
    };

    Map<Var, PseudoCostData> m_pseudo_cost_data;

    double compute_upper_bounding_score(const Var& t_var, const PseudoCostData& t_data, const Node<NodeInfoT>& t_node) const;

    double compute_lower_bounding_score(const Var& t_var, const PseudoCostData& t_data, const Node<NodeInfoT>& t_node) const;

    double compute_average_lower_bounding_score(const Node<NodeInfoT>& t_node) const;

    double compute_average_upper_bounding_score(const Node<NodeInfoT>& t_node) const;
public:
    explicit PseudoCost(const Optimizers::BranchAndBound<NodeInfoT>& t_parent, std::list<Var> t_branching_candidates);

    std::list<std::pair<Var, double>> scoring_function(const std::list<Var> &t_var, const Node<NodeInfoT> &t_node) override;

    void on_node_solved(const Node<NodeInfoT> &t_node) override;

};

template<class NodeInfoT>
typename idol::BranchingRules::PseudoCost<NodeInfoT>::PseudoCostData &
idol::BranchingRules::PseudoCost<NodeInfoT>::PseudoCostData::operator+=(
        const idol::BranchingRules::PseudoCost<NodeInfoT>::PseudoCostData &t_rhs) {

    objective_gains_by_upper_boundings += t_rhs.objective_gains_by_upper_boundings;
    objective_gains_by_lower_boundings += t_rhs.objective_gains_by_lower_boundings;
    n_entries_for_upper_bounds += t_rhs.n_entries_for_upper_bounds;
    n_entries_for_lower_bounds += t_rhs.n_entries_for_lower_bounds;

    return *this;
}

template<class NodeInfoT>
void idol::BranchingRules::PseudoCost<NodeInfoT>::on_node_solved(const idol::Node<NodeInfoT> &t_node) {

    BranchingRule<NodeInfoT>::on_node_solved(t_node);

    if (t_node.level() == 0) {
        return;
    }

    // Parent
    const auto& parent = t_node.parent();
    const auto& parent_solution = parent.info().primal_solution();
    const double parent_objective_value = parent_solution.objective_value();

    // Current node
    for (const auto &branching_decision : t_node.info().variable_branching_decisions()) {

        const auto &var = branching_decision.variable;
        const double bound = branching_decision.bound;
        const bool is_upper_bound = branching_decision.type == LessOrEqual;
        const double node_objective_value = t_node.info().objective_value();

        const double parent_var_primal_value = parent_solution.get(var);

        const double objective_gain_per_unit_change =
                (node_objective_value - parent_objective_value) / std::abs(bound - parent_var_primal_value);

        PseudoCostData data;

        if (is_upper_bound) {
            data.n_entries_for_upper_bounds = 1;
            data.objective_gains_by_upper_boundings = objective_gain_per_unit_change;
        } else {
            data.n_entries_for_lower_bounds = 1;
            data.objective_gains_by_lower_boundings = objective_gain_per_unit_change;
        }

        auto [it, success] = m_pseudo_cost_data.emplace(var, data);

        if (!success) {
            it->second += data;
        }

    }

}

template<class NodeInfoT>
idol::BranchingRules::PseudoCost<NodeInfoT>::PseudoCost(
        const idol::Optimizers::BranchAndBound<NodeInfoT> &t_parent, std::list<Var> t_branching_candidates)
        : VariableBranching<NodeInfoT>(t_parent, std::move(t_branching_candidates)),
          m_scoring_function(new NodeScoreFunctions::Linear())
{}

template<class NodeInfoT>
std::list<std::pair<idol::Var, double>>
idol::BranchingRules::PseudoCost<NodeInfoT>::scoring_function(const std::list<idol::Var> &t_variables,
                                                                  const Node<NodeInfoT> &t_node) {

    std::list<std::pair<idol::Var, double>> result;

    const double average_upper_bound_score = compute_average_upper_bounding_score(t_node);
    const double average_lower_bound_score = compute_average_lower_bounding_score(t_node);
    const auto end = m_pseudo_cost_data.end();

    for (const auto& var : t_variables) {

        const auto it = m_pseudo_cost_data.find(var);

        const auto upper_bounding_score = it != end && it->second.n_entries_for_upper_bounds > 0 ? compute_upper_bounding_score(it->first, it->second, t_node) : average_upper_bound_score;
        const auto lower_bounding_score = it != end && it->second.n_entries_for_lower_bounds > 0 ? compute_lower_bounding_score(it->first, it->second, t_node) : average_lower_bound_score;

        const double pseudo_cost = (*m_scoring_function)(upper_bounding_score,lower_bounding_score);

        result.emplace_back(var, pseudo_cost);

    }

    return result;
}

template<class NodeInfoT>
double idol::BranchingRules::PseudoCost<NodeInfoT>::compute_lower_bounding_score(const idol::Var &t_var,
                                                                              const PseudoCostData& t_data,
                                                                              const idol::Node<NodeInfoT> &t_node) const {

    const double sum_of_objective_gains_per_unit_change = t_data.objective_gains_by_lower_boundings;
    const unsigned int n_entries = t_data.n_entries_for_lower_bounds;
    const double var_value = t_node.info().primal_solution().get(t_var);

    return (std::ceil(var_value) - var_value) * sum_of_objective_gains_per_unit_change / (double) n_entries;
}

template<class NodeInfoT>
double idol::BranchingRules::PseudoCost<NodeInfoT>::compute_upper_bounding_score(const idol::Var &t_var,
                                                                              const PseudoCostData& t_data,
                                                                              const idol::Node<NodeInfoT> &t_node) const {

    const double sum_of_objective_gains_per_unit_change = t_data.objective_gains_by_upper_boundings;
    const unsigned int n_entries = t_data.n_entries_for_upper_bounds;
    const double var_value = t_node.info().primal_solution().get(t_var);

    return (var_value - std::floor(var_value)) * sum_of_objective_gains_per_unit_change / (double) n_entries;
}

template<class NodeInfoT>
double idol::BranchingRules::PseudoCost<NodeInfoT>::compute_average_upper_bounding_score(
        const idol::Node<NodeInfoT> &t_node) const {

    double sum = 0.;
    unsigned int n = 0;

    for (const auto& [var, data] : m_pseudo_cost_data) {

        if (data.n_entries_for_upper_bounds == 0) {
            continue;
        }

        sum += compute_upper_bounding_score(var, data, t_node);
        n += 1;
    }

    if (n == 0) {
        return 0.;
    }

    return sum / (double) n;
}

template<class NodeInfoT>
double idol::BranchingRules::PseudoCost<NodeInfoT>::compute_average_lower_bounding_score(
        const idol::Node<NodeInfoT> &t_node) const {

    double sum = 0.;
    unsigned int n = 0;

    for (const auto& [var, data] : m_pseudo_cost_data) {

        if (data.n_entries_for_lower_bounds == 0) {
            continue;
        }

        sum += compute_lower_bounding_score(var, data, t_node);
        n += 1;
    }

    return sum / (double) n;
}

#endif //IDOL_IMPL_PSEUDOCOST_H
