//
// Created by henri on 17.10.23.
//

#ifndef IDOL_IMPL_DIVER_H
#define IDOL_IMPL_DIVER_H

#include <cmath>
#include "VariableBranching.h"

namespace idol::BranchingRules {
    template<class BranchingRuleT> class Diver;
}

template<class BranchingRuleT>
class idol::BranchingRules::Diver : public BranchingRuleT {
public:
    using NodeInfoT = typename BranchingRuleT::NodeInfoT;

    explicit Diver(const Optimizers::BranchAndBound<NodeInfoT>& t_parent, std::list<Var> t_branching_candidates);

    std::list<NodeInfoT *> create_child_nodes(const Node<NodeInfoT> &t_node) override;
};

template<class BranchingRuleT>
idol::BranchingRules::Diver<BranchingRuleT>::Diver(const idol::Optimizers::BranchAndBound<NodeInfoT> &t_parent,
                                                   std::list<Var> t_branching_candidates)
                                                   : BranchingRuleT(t_parent, std::move(t_branching_candidates)) {}

template<class BranchingRuleT>
std::list<typename BranchingRuleT::NodeInfoT *>
idol::BranchingRules::Diver<BranchingRuleT>::create_child_nodes(const idol::Node<NodeInfoT> &t_node) {

    auto children = BranchingRuleT::create_child_nodes(t_node);

    auto it = children.begin();
    const auto end = children.end();

    auto first = *it;

    for (++it ; it != end ; ++it) {
        delete *it;
    }

    return { first };
}

#endif //IDOL_IMPL_DIVER_H
