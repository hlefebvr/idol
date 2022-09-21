//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_NODESTORAGESTRATEGY_H
#define OPTIMIZE_NODESTORAGESTRATEGY_H

#include "AbstractNodeStorageStartegy.h"
#include "modeling/numericals.h"
#include "AbstractSolutionStrategy.h"
#include "algorithms/logs/Log.h"
#include <list>
#include <vector>
#include <algorithm>

template<class NodeT>
class NodeStorageStrategy : public AbstractNodeStorageStrategy {
    std::list<NodeT*> m_solution_pool;
    std::vector<NodeT*> m_active_nodes;
    std::list<NodeT*> m_nodes_to_be_processed;

    NodeT* m_best_upper_bound_node = nullptr;
    NodeT* m_current_node = nullptr;

    template<class T> void free(T& t_container);
public:
    ~NodeStorageStrategy() override;

    void initialize() override;

    bool has_current_node() override;

    [[nodiscard]] const AbstractNode &current_node() const override;

    [[nodiscard]] bool has_node_to_be_processed() const override;

    void set_current_node_to_next_node_to_be_processed() override;

    void save_current_node_solution(const AbstractSolutionStrategy &t_solution_strategy) override;

    void add_node_to_be_processed(AbstractNode *t_node) override;

    void set_current_node_as_incumbent() override;

    void add_current_node_to_solution_pool() override;

    void reset_current_node() override;

    void prune_current_node() override;

    void add_current_node_to_active_nodes() override;

    void prune_active_nodes_by_bound(double t_upper_bound) override;

    bool has_no_active_nodes() override;

    const AbstractNode &select_node_for_branching() override;

    [[nodiscard]] bool has_incumbent() const override;

    [[nodiscard]] const AbstractNode &incumbent() const override;

    const AbstractNode &lowest_node() override;
};

template<class NodeT>
template<class T>
void NodeStorageStrategy<NodeT>::free(T& t_container) {
    for (auto* t_ptr : t_container) { delete t_ptr; }
    t_container = T();
}

template<class NodeT>
NodeStorageStrategy<NodeT>::~NodeStorageStrategy() {
    free(m_active_nodes);
    free(m_nodes_to_be_processed);
    free(m_solution_pool);
    delete m_current_node;
}

template<class NodeT>
void NodeStorageStrategy<NodeT>::initialize() {
    m_best_upper_bound_node = nullptr;
    m_current_node = nullptr;
    free(m_active_nodes);
    free(m_nodes_to_be_processed);
    free(m_solution_pool);
}

template<class NodeT>
const AbstractNode &NodeStorageStrategy<NodeT>::current_node() const {
    return *m_current_node;
}

template<class NodeT>
bool NodeStorageStrategy<NodeT>::has_node_to_be_processed() const {
    return !m_nodes_to_be_processed.empty();
}

template<class NodeT>
void NodeStorageStrategy<NodeT>::set_current_node_to_next_node_to_be_processed() {
    m_current_node = m_nodes_to_be_processed.back();
    m_nodes_to_be_processed.pop_back();
}

template<class NodeT>
void NodeStorageStrategy<NodeT>::save_current_node_solution(const AbstractSolutionStrategy & t_solution_strategy){
    m_current_node->save_solution(t_solution_strategy);
}

template<class NodeT>
bool NodeStorageStrategy<NodeT>::has_current_node() {
    return m_current_node;
}

template<class NodeT>
void NodeStorageStrategy<NodeT>::add_node_to_be_processed(AbstractNode *t_node) {
    m_nodes_to_be_processed.emplace_back(dynamic_cast<NodeT*>(t_node));
}

template<class NodeT>
void NodeStorageStrategy<NodeT>::set_current_node_as_incumbent() {
    m_best_upper_bound_node = m_current_node;
}

template<class NodeT>
void NodeStorageStrategy<NodeT>::add_current_node_to_solution_pool() {
    m_solution_pool.emplace_back(m_current_node);
}

template<class NodeT>
void NodeStorageStrategy<NodeT>::reset_current_node() {
    m_current_node = nullptr;
}

template<class NodeT>
void NodeStorageStrategy<NodeT>::prune_current_node() {
    delete m_current_node;
    m_current_node = nullptr;
}

template<class NodeT>
void NodeStorageStrategy<NodeT>::add_current_node_to_active_nodes() {
    m_active_nodes.emplace_back(m_current_node);
    std::push_heap(m_active_nodes.begin(), m_active_nodes.end(), std::less<AbstractNode*>());
    m_current_node = nullptr;
}

template<class NodeT>
void NodeStorageStrategy<NodeT>::prune_active_nodes_by_bound(double t_upper_bound) {
    for (auto it = m_active_nodes.begin(), end = m_active_nodes.end() ; it != end ; ++it) {
        const auto* ptr_to_node = *it;
        if (ptr_to_node->objective_value() >= t_upper_bound) {
            EASY_LOG(Trace, "branch-and-bound", "[NODE_PRUNED] value = node " << ptr_to_node->id() << ".");
            delete ptr_to_node;
            it = m_active_nodes.erase(it);
        }
    }
    std::make_heap(m_active_nodes.begin(), m_active_nodes.end(), std::less<AbstractNode*>());
}

template<class NodeT>
const AbstractNode &NodeStorageStrategy<NodeT>::lowest_node() {
    return *m_active_nodes.front();
}

template<class NodeT>
bool NodeStorageStrategy<NodeT>::has_no_active_nodes() {
    return m_active_nodes.empty();
}

template<class NodeT>
const AbstractNode &NodeStorageStrategy<NodeT>::select_node_for_branching() {
    auto* selected_node = m_active_nodes.front();
    std::pop_heap(m_active_nodes.begin(), m_active_nodes.end(), std::less<AbstractNode*>());
    m_active_nodes.pop_back(); // TODO this should be done later (at the same time as delete)
    return *selected_node;
}

template<class NodeT>
bool NodeStorageStrategy<NodeT>::has_incumbent() const {
    return m_best_upper_bound_node;
}

template<class NodeT>
const AbstractNode &NodeStorageStrategy<NodeT>::incumbent() const {
    return *m_best_upper_bound_node;
}

#endif //OPTIMIZE_NODESTORAGESTRATEGY_H
