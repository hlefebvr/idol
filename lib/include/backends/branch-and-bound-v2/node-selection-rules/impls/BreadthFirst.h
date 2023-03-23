//
// Created by henri on 22/03/23.
//

#ifndef IDOL_BREADTHFIRST_IMPL_H
#define IDOL_BREADTHFIRST_IMPL_H

#include "NodeSelectionRule.h"
#include "backends/branch-and-bound-v2/NodeSet.h"

namespace NodeSelectionRules {
    template<class NodeT> class BreadthFirst;
}

template<class NodeT>
class NodeSelectionRules::BreadthFirst : public NodeSelectionRule<NodeT> {
public:
    typename NodeSet<Node2<NodeT>>::const_iterator operator()(const NodeSet<Node2<NodeT>>& t_active_nodes) const override {
        return t_active_nodes.by_level().begin();
    }
};

#endif //IDOL_BREADTHFIRST_IMPL_H
