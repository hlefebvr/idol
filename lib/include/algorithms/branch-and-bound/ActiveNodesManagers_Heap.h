//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_ACTIVENODESMANAGERS_HEAP_H
#define OPTIMIZE_ACTIVENODESMANAGERS_HEAP_H

#include "ActiveNodesManager.h"
#include "algorithms/logs/Log.h"
#include <vector>
#include <algorithm>

namespace ActiveNodesManagers {
    class Heap;
}

class ActiveNodesManagers::Heap {
public:

    template<class NodeT>
    class Strategy : public ActiveNodesManagerWithNodeType<NodeT> {
        std::vector<NodeT*> m_nodes;
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
    m_nodes.emplace_back(t_node);
    std::push_heap(m_nodes.begin(), m_nodes.end(), std::greater<Node*>());
}

template<class NodeT>
void ActiveNodesManagers::Heap::Strategy<NodeT>::prune_by_bound(double t_upper_bound) {

    auto it = m_nodes.begin();
    auto end = m_nodes.end();

    while (it != end) {

        if (const auto* ptr_to_node = *it ; ptr_to_node->objective_value() >= t_upper_bound) {
            EASY_LOG(Trace, "branch-and-bound", "[NODE_PRUNED] value = node " << ptr_to_node->id() << ".");
            delete ptr_to_node;
            it = m_nodes.erase(it);
            end = m_nodes.end();
        } else {
            ++it;
        }

    }
    std::make_heap(m_nodes.begin(), m_nodes.end(), std::greater<Node*>());
}

template<class NodeT>
const NodeT &ActiveNodesManagers::Heap::Strategy<NodeT>::lowest_node() const {
    return *m_nodes.front();
}

template<class NodeT>
bool ActiveNodesManagers::Heap::Strategy<NodeT>::empty() const {
    return m_nodes.empty();
}

template<class NodeT>
const NodeT &ActiveNodesManagers::Heap::Strategy<NodeT>::node_selected_for_branching() const {
    return *m_nodes.front();
}

template<class NodeT>
void ActiveNodesManagers::Heap::Strategy<NodeT>::remove_node_selected_for_branching() {
    std::pop_heap(m_nodes.begin(), m_nodes.end(), std::greater<Node*>());
    delete m_nodes.back();
    m_nodes.pop_back();

}

template<class NodeT>
ActiveNodesManagers::Heap::Strategy<NodeT>::~Strategy() {
    for (auto* ptr : m_nodes) { delete ptr; }
}

template<class NodeT>
void ActiveNodesManagers::Heap::Strategy<NodeT>::initialize() {
    for (auto* ptr : m_nodes) { delete ptr; }
    m_nodes.clear();
}

template<class NodeT>
void ActiveNodesManagers::Heap::Strategy<NodeT>::select_node_for_branching() {
    EASY_LOG(Trace, "branch-and-bound", "Node " << node_selected_for_branching().id() << " has been selected for branching.");
}

#endif //OPTIMIZE_ACTIVENODESMANAGERS_HEAP_H
