//
// Created by henri on 22/03/23.
//

#ifndef IDOL_BREADTHFIRST_IMPL_H
#define IDOL_BREADTHFIRST_IMPL_H

#include "NodeSelectionRule.h"
#include "optimizers/branch-and-bound/NodeSet.h"

namespace NodeSelectionRules {
    template<class NodeT> class BreadthFirst;
}

template<class NodeT>
class NodeSelectionRules::BreadthFirst : public NodeSelectionRule<NodeT> {
public:
    typename NodeSet<Node<NodeT>>::const_iterator operator()(const NodeSet<Node<NodeT>>& t_active_nodes) const override {
        return t_active_nodes.by_level().begin();
    }
};

#endif //IDOL_BREADTHFIRST_IMPL_H
