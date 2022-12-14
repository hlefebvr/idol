//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_ACTIVENODESMANAGERS_HEAP_H
#define OPTIMIZE_ACTIVENODESMANAGERS_HEAP_H

#include "ActiveNodesManager.h"
#include "algorithms/parameters/Logs.h"
#include "NodeSet.h"
#include "Attributes_BranchAndBound.h"
#include <algorithm>
#include <variant>

namespace ActiveNodesManagers {
    class Basic;
}

class ActiveNodesManagers::Basic {
public:

    template<class NodeT>
    class Strategy : public ActiveNodesManagerWithNodeType<NodeT> {
        BranchAndBound& m_parent;
        NodeSet<NodeT> m_nodes;
        typename NodeSet<NodeT>::const_iterator m_node_selected_for_branching;

        void select_node_for_branching(int t_strategy);
        virtual void automatically_select_node_for_branching();
    public:
        explicit Strategy(BranchAndBound& t_parent) : m_parent(t_parent) {}

        ~Strategy() override;

        void initialize() override;

        void add(NodeT *t_node);

        void prune_by_bound(double t_upper_bound) override;

        [[nodiscard]] const NodeT& lowest_node() const override;

        [[nodiscard]] bool empty() const override;

        [[nodiscard]] const NodeT &node_selected_for_branching() const override;

        void remove_node_selected_for_branching() override;

        void select_node_for_branching() override;

        unsigned int size() const override;
    };

};

template<class NodeT>
void ActiveNodesManagers::Basic::Strategy<NodeT>::add(NodeT *t_node) {
    m_nodes.emplace(t_node);
}

template<class NodeT>
void ActiveNodesManagers::Basic::Strategy<NodeT>::prune_by_bound(double t_upper_bound) {

    auto it = m_nodes.by_objective_value().begin();
    auto end = m_nodes.by_objective_value().end();

    while (it != end) {

        if (const auto& node = *it ; node.objective_value() >= t_upper_bound) {
            idol_Log(
                    Trace,
                    BranchAndBound,
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
const NodeT &ActiveNodesManagers::Basic::Strategy<NodeT>::lowest_node() const {
    return *m_nodes.by_objective_value().begin();
}

template<class NodeT>
bool ActiveNodesManagers::Basic::Strategy<NodeT>::empty() const {
    return m_nodes.empty();
}

template<class NodeT>
const NodeT &ActiveNodesManagers::Basic::Strategy<NodeT>::node_selected_for_branching() const {
    return *m_node_selected_for_branching;
}

template<class NodeT>
void ActiveNodesManagers::Basic::Strategy<NodeT>::remove_node_selected_for_branching() {
    auto* ptr = m_node_selected_for_branching.operator->();
    m_nodes.erase(m_node_selected_for_branching);
    delete ptr;
}

template<class NodeT>
ActiveNodesManagers::Basic::Strategy<NodeT>::~Strategy() {
    for (auto& node : m_nodes.by_objective_value()) { delete &node; }
}

template<class NodeT>
void ActiveNodesManagers::Basic::Strategy<NodeT>::initialize() {
    for (auto& node : m_nodes.by_objective_value()) { delete &node; }
    m_nodes.clear();
}

template<class NodeT>
void ActiveNodesManagers::Basic::Strategy<NodeT>::select_node_for_branching(int t_strategy) {

    switch (t_strategy) {
        case NodeSelections::Automatic:
            automatically_select_node_for_branching();
            break;
        case NodeSelections::WorstBound:
            m_node_selected_for_branching = m_nodes.by_objective_value().begin();
            break;
        case NodeSelections::BestBound:
            m_node_selected_for_branching = --m_nodes.by_objective_value().end();
            break;
        case NodeSelections::DepthFirst:
            m_node_selected_for_branching = --m_nodes.by_level().end();
            break;
        case NodeSelections::BreadthFirst:
            m_node_selected_for_branching = m_nodes.by_level().begin();
            break;
        default: throw Exception("Unknown specified node selection strategy.");
    }

}

template<class NodeT>
void ActiveNodesManagers::Basic::Strategy<NodeT>::automatically_select_node_for_branching() {
    if (m_parent.relative_gap() <= 3e-2) {
        return select_node_for_branching(NodeSelections::DepthFirst);
    }
    if (m_parent.relative_gap() >= 2e-1) {
        return select_node_for_branching(NodeSelections::DepthFirst);
    }
    return select_node_for_branching(NodeSelections::WorstBound);
}

template<class NodeT>
void ActiveNodesManagers::Basic::Strategy<NodeT>::select_node_for_branching() {
    select_node_for_branching(m_parent.get(Param::BranchAndBound::NodeSelection));
    idol_Log(Trace, BranchAndBound, "Node " << node_selected_for_branching().id() << " has been selected for branching.");
}

template<class NodeT>
unsigned int ActiveNodesManagers::Basic::Strategy<NodeT>::size() const {
    return m_nodes.size();
}

#endif //OPTIMIZE_ACTIVENODESMANAGERS_HEAP_H
