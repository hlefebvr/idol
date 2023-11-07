//
// Created by henri on 17.10.23.
//

#ifndef IDOL_IMPL_STRONGBRANCHING_H
#define IDOL_IMPL_STRONGBRANCHING_H

#include "VariableBranching.h"
#include "NodeScoreFunction.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/strong-branching/StrongBranchingPhase.h"
#include "MostInfeasible.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/StrongBranching.h"

namespace idol::BranchingRules {
    template<class NodeVarInfoT> class StrongBranching;
}

template<class NodeVarInfoT>
class idol::BranchingRules::StrongBranching : public VariableBranching<NodeVarInfoT> {
    std::unique_ptr<VariableBranching<NodeVarInfoT>> m_inner_variable_branching_rule;
    std::unique_ptr<NodeScoreFunction> m_score_function;
    std::list<StrongBranchingPhase> m_phases;

    std::vector<std::pair<Var, double>> sort_variables_by_score(const std::list<std::pair<Var, double>>& t_scores);

    std::list<Node<NodeVarInfoT>> make_nodes(const std::list<NodeVarInfoT*>& t_node_infos, const Node<NodeVarInfoT>& t_parent_node);

    StrongBranchingPhase& current_phase(const Node<NodeVarInfoT>& t_node);

    void solve_nodes(StrongBranchingPhase& t_phase, std::list<Node<NodeVarInfoT>>& t_nodes);

    double compute_score(double t_parent_objective, std::list<Node<NodeVarInfoT>>& t_nodes);
public:
    explicit StrongBranching(const Optimizers::BranchAndBound<NodeVarInfoT>& t_parent,
                             std::list<Var> t_branching_candidates,
                             unsigned int t_max_n_variables,
                             NodeScoreFunction* t_score_function,
                             const std::list<StrongBranchingPhase>& t_phases);

    std::list<std::pair<Var, double>> scoring_function(const std::list<Var> &t_var, const Node<NodeVarInfoT> &t_node) override;
};

template<class NodeVarInfoT>
idol::BranchingRules::StrongBranching<NodeVarInfoT>::StrongBranching(
        const idol::Optimizers::BranchAndBound<NodeVarInfoT> &t_parent,
        std::list<Var> t_branching_candidates,
        unsigned int t_max_n_variables,
        NodeScoreFunction* t_score_function,
        const std::list<StrongBranchingPhase>& t_phases
        )
        : VariableBranching<NodeVarInfoT>(t_parent, std::move(t_branching_candidates)),
          m_inner_variable_branching_rule(new BranchingRules::MostInfeasible<NodeVarInfoT>(t_parent, {})),
          m_score_function(t_score_function->clone()),
          m_phases(t_phases)
{

      // add last phase
      m_phases.emplace_back(
              StrongBranchingPhases::WithNodeOptimizer(),
              t_max_n_variables,
              std::numeric_limits<unsigned int>::max()
      );
}

template<class NodeVarInfoT>
std::list<std::pair<idol::Var, double>>
idol::BranchingRules::StrongBranching<NodeVarInfoT>::scoring_function(const std::list<idol::Var> &t_variables,
                                                                   const Node<NodeVarInfoT> &t_node) {

    std::list<std::pair<Var, double>> result;

    auto& phase = current_phase(t_node);
    const auto scores = m_inner_variable_branching_rule->scoring_function(t_variables, t_node);
    const auto sorted_scores = sort_variables_by_score(scores);
    const unsigned int n_nodes_to_solve = std::min<unsigned int>(phase.max_n_variables(), sorted_scores.size());

    const double objective_value_parent_node = t_node.info().objective_value();

    for (unsigned int k = 0 ; k < n_nodes_to_solve ; ++k) {

        const auto branching_candidate = sorted_scores[k].first;

        auto node_infos = m_inner_variable_branching_rule->create_child_nodes_for_selected_variable(t_node, branching_candidate);
        auto nodes = make_nodes(node_infos, t_node);

        solve_nodes(phase, nodes);

        result.emplace_back(branching_candidate, compute_score(objective_value_parent_node, nodes));

    }

    return result;
}

template<class NodeVarInfoT>
std::vector<std::pair<idol::Var, double>> idol::BranchingRules::StrongBranching<NodeVarInfoT>::sort_variables_by_score(
        const std::list<std::pair<Var, double>> &t_scores) {

    std::vector<std::pair<Var, double>> result;
    result.reserve(t_scores.size());

    std::copy(t_scores.begin(), t_scores.end(), std::back_inserter(result));

    std::sort(result.begin(), result.end(), [](const auto& t_a, const auto& t_b) {
        return t_a.second > t_b.second;
    });

    return result;
}
template<class NodeVarInfoT>
std::list<idol::Node<NodeVarInfoT>>
idol::BranchingRules::StrongBranching<NodeVarInfoT>::make_nodes(const std::list<NodeVarInfoT*>& t_node_infos,
                                                             const Node<NodeVarInfoT>& t_parent_node) {

    std::list<idol::Node<NodeVarInfoT>> result;

    const unsigned int id = t_parent_node.id();

    for (auto* info : t_node_infos) {
        result.emplace_back(info, id, t_parent_node);
    }

    return result;
}

template<class NodeVarInfoT>
void idol::BranchingRules::StrongBranching<NodeVarInfoT>::solve_nodes(StrongBranchingPhase& t_phase, std::list<Node<NodeVarInfoT>>& t_nodes) {

    auto& branch_and_bound = this->parent();
    auto& optimizer = const_cast<Optimizer&>(branch_and_bound.relaxation().optimizer());

    for (auto& node : t_nodes) {

        t_phase.type().build(optimizer);

        branch_and_bound.solve(node);

        t_phase.type().clean(optimizer);
    }

}

template<class NodeVarInfoT>
double idol::BranchingRules::StrongBranching<NodeVarInfoT>::compute_score(double t_parent_objective,
                                                                       std::list<Node<NodeVarInfoT>>& t_nodes) {

    if (t_nodes.size() != 2) {
        throw Exception("Strong branching expected two nodes, got " + std::to_string(t_nodes.size()) + ".");
    }

    const auto& left_node_info = t_nodes.front().info();
    const auto& right_node_info = t_nodes.back().info();

    const double left_objective_value = left_node_info.has_objective_value() ? left_node_info.objective_value() : Inf;
    const double right_objective_value = right_node_info.has_objective_value() ? right_node_info.objective_value() : Inf;

    return m_score_function->operator()(
            left_objective_value - t_parent_objective,
            right_objective_value - t_parent_objective);

}

template<class NodeVarInfoT>
idol::StrongBranchingPhase &
idol::BranchingRules::StrongBranching<NodeVarInfoT>::current_phase(const idol::Node<NodeVarInfoT> &t_node) {

    const unsigned int level = t_node.level();

    for (auto& phase : m_phases) {
        if (level <= phase.max_depth()) {
            return phase;
        }
    }

    throw Exception("Could not infer strong branching phase.");
}


#endif //IDOL_IMPL_STRONGBRANCHING_H
