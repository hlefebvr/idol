//
// Created by henri on 22/03/23.
//

#ifndef IDOL_BESTBOUND_IMPL_H
#define IDOL_BESTBOUND_IMPL_H

#include "NodeSelectionRule.h"
#include "backends/branch-and-bound-v2/NodeSet.h"

namespace NodeSelectionRules {
    template<class NodeT> class BestBound;
}

template<class NodeT>
class NodeSelectionRules::BestBound : public NodeSelectionRule<NodeT> {
public:
    typename NodeSet<Node2<NodeT>>::const_iterator operator()(const NodeSet<Node2<NodeT>>& t_active_nodes) const override {
        return --t_active_nodes.by_objective_value().end();
    }
};

#endif //IDOL_BESTBOUND_IMPL_H
