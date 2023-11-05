//
// Created by henri on 22/03/23.
//

#ifndef IDOL_WORSTBOUND_IMPL_H
#define IDOL_WORSTBOUND_IMPL_H

#include "NodeSelectionRule.h"
#include "idol/optimizers/branch-and-bound/nodes/NodeSet.h"

namespace idol::NodeSelectionRules {
    template<class NodeT> class WorstBound;
}

template<class NodeT>
class idol::NodeSelectionRules::WorstBound : public NodeSelectionRule<NodeT> {
public:
    explicit WorstBound(Optimizers::BranchAndBound<NodeT>& t_parent) : NodeSelectionRule<NodeT>(t_parent) {}

    typename NodeSet<Node<NodeT>>::const_iterator operator()(const NodeSet<Node<NodeT>>& t_active_nodes) override {
        return --t_active_nodes.by_objective_value().end();
    }
};

#endif //IDOL_WORSTBOUND_IMPL_H
