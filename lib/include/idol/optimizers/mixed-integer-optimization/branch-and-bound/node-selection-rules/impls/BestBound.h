//
// Created by henri on 22/03/23.
//

#ifndef IDOL_BESTBOUND_IMPL_H
#define IDOL_BESTBOUND_IMPL_H

#include "NodeSelectionRule.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/nodes/NodeSet.h"

namespace idol::NodeSelectionRules {
    template<class NodeT> class BestBound;
}

template<class NodeT>
class idol::NodeSelectionRules::BestBound : public NodeSelectionRule<NodeT> {
public:
    explicit BestBound(Optimizers::BranchAndBound<NodeT>& t_parent) : NodeSelectionRule<NodeT>(t_parent) {}

    typename NodeSet<Node<NodeT>>::const_iterator operator()(const NodeSet<Node<NodeT>>& t_active_nodes) override {
        return t_active_nodes.by_objective_value().begin();
    }
};

#endif //IDOL_BESTBOUND_IMPL_H
