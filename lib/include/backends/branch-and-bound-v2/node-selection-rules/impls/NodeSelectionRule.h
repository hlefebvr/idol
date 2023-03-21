//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODESELECTIONRULE_H
#define IDOL_NODESELECTIONRULE_H

#include "../../nodes/Node2.h"

template<class NodeT>
class NodeSelectionRule {
public:
    virtual ~NodeSelectionRule() = default;

    virtual typename NodeSet<Node2<NodeT>>::const_iterator operator()(const NodeSet<Node2<NodeT>>& t_active_nodes) const = 0;
};

#endif //IDOL_NODESELECTIONRULE_H
