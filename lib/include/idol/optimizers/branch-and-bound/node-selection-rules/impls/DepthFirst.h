//
// Created by henri on 21/03/23.
//

#ifndef IDOL_DEPTHFIRST_IMPL_H
#define IDOL_DEPTHFIRST_IMPL_H

#include "NodeSelectionRule.h"
#include "idol/optimizers/branch-and-bound/NodeSet.h"

namespace idol::NodeSelectionRules {
    template<class NodeT> class DepthFirst;
}

template<class NodeT>
class idol::NodeSelectionRules::DepthFirst : public NodeSelectionRule<NodeT> {
public:
    explicit DepthFirst(Optimizers::BranchAndBound<NodeT>& t_parent) : NodeSelectionRule<NodeT>(t_parent) {}

    typename NodeSet<Node<NodeT>>::const_iterator operator()(const NodeSet<Node<NodeT>>& t_active_nodes) override {
        return --t_active_nodes.by_level().end();
    }
};

#endif //IDOL_DEPTHFIRST_IMPL_H
