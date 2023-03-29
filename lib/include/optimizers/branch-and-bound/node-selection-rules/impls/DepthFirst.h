//
// Created by henri on 21/03/23.
//

#ifndef IDOL_DEPTHFIRST_IMPL_H
#define IDOL_DEPTHFIRST_IMPL_H

#include "NodeSelectionRule.h"
#include "optimizers/branch-and-bound/NodeSet.h"

namespace NodeSelectionRules {
    template<class NodeT> class DepthFirst;
}

template<class NodeT>
class NodeSelectionRules::DepthFirst : public NodeSelectionRule<NodeT> {
public:
    typename NodeSet<Node<NodeT>>::const_iterator operator()(const NodeSet<Node<NodeT>>& t_active_nodes) const override {
        return --t_active_nodes.by_level().end();
    }
};

#endif //IDOL_DEPTHFIRST_IMPL_H
