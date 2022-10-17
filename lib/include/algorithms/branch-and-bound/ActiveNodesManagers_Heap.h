//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_ACTIVENODESMANAGERS_HEAP_H
#define OPTIMIZE_ACTIVENODESMANAGERS_HEAP_H

#include "ActiveNodesManager.h"
#include "../logs/Log.h"
#include "NodeSet.h"
#include <algorithm>
#include <variant>

namespace ActiveNodesManagers {
    class Heap;
}

class ActiveNodesManagers::Heap {
public:

    template<class NodeT>
    class Strategy : public ActiveNodesManagerWithNodeType<NodeT> {
        NodeSet<NodeT> m_nodes;
        typename NodeSet<NodeT>::const_iterator m_node_selected_for_branching;
    public:
        ~Strategy() override;

        void initialize() override;

        void add(NodeT *t_node);

        void prune_by_bound(double t_upper_bound) override;

        [[nodiscard]] const NodeT& lowest_node() const override;

        [[nodiscard]] bool empty() const override;

        [[nodiscard]] const NodeT &node_selected_for_branching() const override;

        void remove_node_selected_for_branching() override;

        void select_node_for_branching() override;
    };

};

template<class NodeT>
void ActiveNodesManagers::Heap::Strategy<NodeT>::add(NodeT *t_node) {
    m_nodes.emplace(t_node);
}

template<class NodeT>
void ActiveNodesManagers::Heap::Strategy<NodeT>::prune_by_bound(double t_upper_bound) {

    auto it = m_nodes.by_objective_value().begin();
    auto end = m_nodes.by_objective_value().end();

    while (it != end) {

        if (const auto& node = *it ; node.objective_value() >= t_upper_bound) {
            EASY_LOG(
                    Trace,
                    "branch-and-bound",
                    "Node " << node.id() << " was pruned by bound."
                    << "Best UB: " << t_upper_bound << ", Obj: " << node.objective_value() << ".");
            it = m_nodes.erase(it);
            end = m_nodes.by_objective_value().end();
            delete &node;
        } else {
            break;
        }

    }
}

template<class NodeT>
const NodeT &ActiveNodesManagers::Heap::Strategy<NodeT>::lowest_node() const {
    return *m_nodes.by_objective_value().begin();
}

template<class NodeT>
bool ActiveNodesManagers::Heap::Strategy<NodeT>::empty() const {
    return m_nodes.empty();
}

template<class NodeT>
const NodeT &ActiveNodesManagers::Heap::Strategy<NodeT>::node_selected_for_branching() const {
    return *m_node_selected_for_branching;
}

template<class NodeT>
void ActiveNodesManagers::Heap::Strategy<NodeT>::remove_node_selected_for_branching() {
    auto* ptr = m_node_selected_for_branching.operator->();
    m_nodes.erase(m_node_selected_for_branching);
    delete ptr;
}

template<class NodeT>
ActiveNodesManagers::Heap::Strategy<NodeT>::~Strategy() {
    for (auto& node : m_nodes.by_objective_value()) { delete &node; }
}

template<class NodeT>
void ActiveNodesManagers::Heap::Strategy<NodeT>::initialize() {
    for (auto& node : m_nodes.by_objective_value()) { delete &node; }
    m_nodes.clear();
}

template<class NodeT>
void ActiveNodesManagers::Heap::Strategy<NodeT>::select_node_for_branching() {
    m_node_selected_for_branching = m_nodes.by_objective_value().begin(); // Worst bound first
    //m_node_selected_for_branching = --m_nodes.by_objective_value().end(); // Best bound first
    //m_node_selected_for_branching = --m_nodes.by_level().end(); // Depth-first
    //m_node_selected_for_branching = m_nodes.by_level().begin(); // Breadth-first
    EASY_LOG(Trace, "branch-and-bound", "Node " << node_selected_for_branching().id() << " has been selected for branching.");
}

#endif //OPTIMIZE_ACTIVENODESMANAGERS_HEAP_H
