//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODESELECTIONRULE_H
#define IDOL_NODESELECTIONRULE_H

#include "idol/optimizers/branch-and-bound/nodes/Node.h"
#include "idol/optimizers/branch-and-bound/nodes/NodeSet.h"

namespace idol {
    namespace Optimizers {
        template<class NodeInfoT>
        class BranchAndBound;
    }

    template<class NodeInfoT> class NodeSelectionRule;
}

template<class NodeInfoT>
class idol::NodeSelectionRule {
    Optimizers::BranchAndBound<NodeInfoT>* m_parent;
public:
    explicit NodeSelectionRule(Optimizers::BranchAndBound<NodeInfoT>& t_parent) : m_parent(&t_parent) {}

    virtual ~NodeSelectionRule() = default;

    virtual typename NodeSet<Node<NodeInfoT>>::const_iterator operator()(const NodeSet<Node<NodeInfoT>>& t_active_nodes) = 0;

    Optimizers::BranchAndBound<NodeInfoT>& parent() { return *m_parent; }

    const Optimizers::BranchAndBound<NodeInfoT>& parent() const { return *m_parent; }
};

#endif //IDOL_NODESELECTIONRULE_H
