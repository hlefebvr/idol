//
// Created by henri on 17.10.23.
//

#ifndef IDOL_IMPL_STRONGBRANCHING_H
#define IDOL_IMPL_STRONGBRANCHING_H

#include "VariableBranching.h"
#include "optimizers/branch-and-bound/branching-rules/impls/strong-branching/NodeScoreFunction.h"

namespace idol::BranchingRules {
    template<class NodeInfoT> class StrongBranching;
}

template<class NodeInfoT>
class idol::BranchingRules::StrongBranching : public VariableBranching<NodeInfoT> {
    std::unique_ptr<VariableBranching<NodeInfoT>> m_inner_variable_branching_rule;
    unsigned int m_max_n_var = 2;
    std::unique_ptr<StrongBranchingScoreFunction> m_score_function;

    std::vector<std::pair<Var, double>> sort_variables_by_score(const std::list<std::pair<Var, double>>& t_scores);

    std::list<Node<NodeInfoT>> make_nodes(const std::list<NodeInfoT*>& t_node_infos, const Node<NodeInfoT>& t_parent_node);

    void solve_nodes(std::list<Node<NodeInfoT>>& t_nodes);

    double compute_score(double t_parent_objective, std::list<Node<NodeInfoT>>& t_nodes);
public:
    explicit StrongBranching(const Optimizers::BranchAndBound<NodeInfoT>& t_parent, std::list<Var> t_branching_candidates);

    std::list<std::pair<Var, double>> scoring_function(const std::list<Var> &t_var, const Node<NodeInfoT> &t_node) override;
};

template<class NodeInfoT>
idol::BranchingRules::StrongBranching<NodeInfoT>::StrongBranching(
        const idol::Optimizers::BranchAndBound<NodeInfoT> &t_parent, std::list<Var> t_branching_candidates)
        : VariableBranching<NodeInfoT>(t_parent, std::move(t_branching_candidates)),
          m_inner_variable_branching_rule(new BranchingRules::MostInfeasible<NodeInfoT>(t_parent, {})),
          m_score_function(new StrongBranchingScores::Product())
{}

template<class NodeInfoT>
std::list<std::pair<idol::Var, double>>
idol::BranchingRules::StrongBranching<NodeInfoT>::scoring_function(const std::list<idol::Var> &t_variables,
                                                                   const Node<NodeInfoT> &t_node) {

    std::list<std::pair<Var, double>> result;

    const auto scores = m_inner_variable_branching_rule->scoring_function(t_variables, t_node);
    const auto sorted_scores = sort_variables_by_score(scores);
    const unsigned int n_nodes_to_solve = std::min<unsigned int>(m_max_n_var, sorted_scores.size());

    const double objective_value_parent_node = t_node.info().objective_value();

    for (unsigned int k = 0 ; k < n_nodes_to_solve ; ++k) {

        const auto branching_candidate = sorted_scores[k].first;

        std::cout << "Should solve node_infos for " << branching_candidate  << ", score = " << sorted_scores[k].second << std::endl;

        auto node_infos = m_inner_variable_branching_rule->create_child_nodes_for_selected_variable(t_node, branching_candidate);
        auto nodes = make_nodes(node_infos, t_node);

        solve_nodes(nodes);

        result.emplace_back(branching_candidate, compute_score(objective_value_parent_node, nodes));

    }

    return result;
}

template<class NodeInfoT>
std::vector<std::pair<idol::Var, double>> idol::BranchingRules::StrongBranching<NodeInfoT>::sort_variables_by_score(
        const std::list<std::pair<Var, double>> &t_scores) {

    std::vector<std::pair<Var, double>> result;
    result.reserve(t_scores.size());

    std::copy(t_scores.begin(), t_scores.end(), std::back_inserter(result));

    std::sort(result.begin(), result.end(), [](const auto& t_a, const auto& t_b) {
        return t_a.second > t_b.second;
    });

    return result;
}
template<class NodeInfoT>
std::list<idol::Node<NodeInfoT>>
idol::BranchingRules::StrongBranching<NodeInfoT>::make_nodes(const std::list<NodeInfoT*>& t_node_infos,
                                                             const Node<NodeInfoT>& t_parent_node) {

    std::list<idol::Node<NodeInfoT>> result;

    const unsigned int id = t_parent_node.id();
    const unsigned int level = t_parent_node.level();

    for (auto* info : t_node_infos) {
        result.emplace_back(info, id, level);
    }

    return result;
}

template<class NodeInfoT>
void idol::BranchingRules::StrongBranching<NodeInfoT>::solve_nodes(std::list<Node<NodeInfoT>>& t_nodes) {

    for (auto& node : t_nodes) {
        this->parent().solve(node);
    }

}

template<class NodeInfoT>
double idol::BranchingRules::StrongBranching<NodeInfoT>::compute_score(double t_parent_objective,
                                                                       std::list<Node<NodeInfoT>>& t_nodes) {

    if (t_nodes.size() != 2) {
        throw Exception("Strong branching expected two nodes, got " + std::to_string(t_nodes.size()) + ".");
    }

    const auto& left_node_info = t_nodes.front().info();
    const auto& right_node_info = t_nodes.back().info();

    const double left_objective_value = left_node_info.has_objective_value() ? left_node_info.objective_value() : -Inf;
    const double right_objective_value = right_node_info.has_objective_value() ? right_node_info.objective_value() : -Inf;

    return m_score_function->operator()(
            t_parent_objective,
            left_objective_value,
            right_objective_value);

}

#endif //IDOL_IMPL_STRONGBRANCHING_H
