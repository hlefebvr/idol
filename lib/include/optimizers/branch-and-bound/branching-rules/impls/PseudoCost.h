//
// Created by henri on 18.10.23.
//

#ifndef IDOL_IMPL_PSEUDOCOST_H
#define IDOL_IMPL_PSEUDOCOST_H

#include <cmath>
#include "VariableBranching.h"

namespace idol::BranchingRules {
    template<class NodeInfoT> class PseudoCost;
}

template<class NodeInfoT>
class idol::BranchingRules::PseudoCost : public VariableBranching<NodeInfoT> {
public:
    explicit PseudoCost(const Optimizers::BranchAndBound<NodeInfoT>& t_parent, std::list<Var> t_branching_candidates);

    std::list<std::pair<Var, double>> scoring_function(const std::list<Var> &t_var, const Node<NodeInfoT> &t_node) override;

    void on_node_solved(const Node<NodeInfoT> &t_node) override {

        BranchingRule<NodeInfoT>::on_node_solved(t_node);

        std::cout << "New node solved! " << t_node.id() << std::endl;

        if (t_node.level() > 0) {
            std::cout << "His parent was " << t_node.parent().id() << std::endl;
        }

    }

};

template<class NodeInfoT>
idol::BranchingRules::PseudoCost<NodeInfoT>::PseudoCost(
        const idol::Optimizers::BranchAndBound<NodeInfoT> &t_parent, std::list<Var> t_branching_candidates)
        : VariableBranching<NodeInfoT>(t_parent, std::move(t_branching_candidates)) {}

template<class NodeInfoT>
std::list<std::pair<idol::Var, double>>
idol::BranchingRules::PseudoCost<NodeInfoT>::scoring_function(const std::list<idol::Var> &t_variables,
                                                                  const Node<NodeInfoT> &t_node) {

    const auto& primal_solution = t_node.info().primal_solution();

    throw Exception("Pseudo cost not implemented.");
}

#endif //IDOL_IMPL_PSEUDOCOST_H
