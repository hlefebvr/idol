//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_ACTIVENODESMANAGER_H
#define OPTIMIZE_ACTIVENODESMANAGER_H

#include "Node.h"

class ActiveNodesManager {
public:
    virtual ~ActiveNodesManager() = default;

    virtual void initialize() = 0;

    virtual void prune_by_bound(double t_upper_bound) = 0;

    [[nodiscard]] virtual const Node& lowest_node() const = 0;

    [[nodiscard]] virtual bool empty() const = 0;

    virtual void select_node_for_branching() = 0;

    [[nodiscard]] virtual const Node& node_selected_for_branching() const = 0;

    virtual void remove_node_selected_for_branching() = 0;

    [[nodiscard]] virtual unsigned int size() const = 0;
};

template<class NodeT>
class ActiveNodesManagerWithNodeType : public ActiveNodesManager {
public:
    virtual void add(NodeT *t_node) = 0;

    [[nodiscard]] const NodeT& lowest_node() const override = 0;

    [[nodiscard]] const NodeT &node_selected_for_branching() const override = 0;
};

#endif //OPTIMIZE_ACTIVENODESMANAGER_H
