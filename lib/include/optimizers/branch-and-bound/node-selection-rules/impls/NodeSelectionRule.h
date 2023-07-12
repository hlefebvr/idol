//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODESELECTIONRULE_H
#define IDOL_NODESELECTIONRULE_H

#include "../../nodes/Node.h"
#include "optimizers/branch-and-bound/NodeSet.h"

template<class NodeT>
class idol::NodeSelectionRule {
public:
    virtual ~NodeSelectionRule() = default;

    virtual typename NodeSet<Node<NodeT>>::const_iterator operator()(const NodeSet<Node<NodeT>>& t_active_nodes) const = 0;
};

#endif //IDOL_NODESELECTIONRULE_H
