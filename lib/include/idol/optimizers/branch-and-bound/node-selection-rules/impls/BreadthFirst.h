//
// Created by henri on 22/03/23.
//

#ifndef IDOL_BREADTHFIRST_IMPL_H
#define IDOL_BREADTHFIRST_IMPL_H

#include "NodeSelectionRule.h"
#include "idol/optimizers/branch-and-bound/NodeSet.h"

namespace idol::NodeSelectionRules {
    template<class NodeT> class BreadthFirst;
}

template<class NodeT>
class idol::NodeSelectionRules::BreadthFirst : public NodeSelectionRule<NodeT> {
public:
    explicit BreadthFirst(Optimizers::BranchAndBound<NodeT>& t_parent) : NodeSelectionRule<NodeT>(t_parent) {}

    typename NodeSet<Node<NodeT>>::const_iterator operator()(const NodeSet<Node<NodeT>>& t_active_nodes) override {
        return t_active_nodes.by_level().begin();
    }
};

#endif //IDOL_BREADTHFIRST_IMPL_H
