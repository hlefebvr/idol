//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_NODESTRATEGIES_BASIC_H
#define OPTIMIZE_NODESTRATEGIES_BASIC_H

#include "NodeStrategy.h"
#include "../../modeling/numericals.h"
#include "../Algorithm.h"
#include "../logs/Log.h"
#include "ActiveNodesManager.h"
#include "NodeUpdator.h"
#include "BranchingStrategy.h"
#include <list>
#include <vector>
#include <algorithm>

namespace NodeStrategies {
    template<class NodeT> class Basic;
}

template<class NodeT>
class NodeStrategies::Basic : public AbstractNodeStrategy {

    unsigned int m_node_id = 0;

    std::list<NodeT*> m_solution_pool;
    std::list<NodeT*> m_nodes_to_be_processed;

    NodeT* m_best_upper_bound_node = nullptr;
    NodeT* m_current_node = nullptr;

    std::unique_ptr<ActiveNodesManagerWithNodeType<NodeT>> m_active_nodes;
    std::unique_ptr<BranchingStrategyWithNodeType<NodeT>> m_branching_strategy;
    std::unique_ptr<NodeUpdatorWithNodeType<NodeT>> m_node_updator;

    template<class T> void free(T& t_container);
public:
    Basic() = default;

    Basic(const Basic&) = delete;
    Basic(Basic&&) noexcept = default;

    ~Basic() override;

    [[nodiscard]] ActiveNodesManagerWithNodeType<NodeT>& active_nodes() override { return *m_active_nodes; }

    [[nodiscard]] const ActiveNodesManagerWithNodeType<NodeT>& active_nodes() const override { return *m_active_nodes; }

    void initialize() override;

    bool has_current_node() override;

    [[nodiscard]] const NodeT &current_node() const override;

    [[nodiscard]] bool has_node_to_be_processed() const override;

    void set_current_node_to_next_node_to_be_processed() override;

    void save_current_node_solution(const Algorithm &t_solution_strategy) override;

    void add_node_to_be_processed(Node *t_node) override;

    void set_current_node_as_incumbent() override;

    void add_current_node_to_solution_pool() override;

    void reset_current_node() override;

    void prune_current_node() override;

    void add_current_node_to_active_nodes() override;

    [[nodiscard]] bool has_incumbent() const override;

    [[nodiscard]] const Node &incumbent() const override;

    bool current_node_has_a_valid_solution() const override;

    unsigned int create_child_nodes() override;

    template<class T, class ...Args>
    typename T::template Strategy<NodeT>& set_active_node_manager_strategy(Args&& ... t_args);

    template<class T, class ...Args>
    typename T::template Strategy<NodeT>& set_branching_strategy(Args&& ... t_args);

    template<class T, class ...Args>
    typename T::template Strategy<NodeT>& set_node_updator_strategy(Args&& ... t_args);

    void create_root_node() override;

    void apply_current_node_to(Algorithm &t_solution_strategy) override;
};

template<class NodeT>
template<class T>
void NodeStrategies::Basic<NodeT>::free(T& t_container) {
    for (auto* t_ptr : t_container) { delete t_ptr; }
    t_container = T();
}

template<class NodeT>
NodeStrategies::Basic<NodeT>::~Basic() {
    free(m_nodes_to_be_processed);
    free(m_solution_pool);
    delete m_current_node;
}

template<class NodeT>
void NodeStrategies::Basic<NodeT>::initialize() {
    m_node_id = 0;
    m_best_upper_bound_node = nullptr;
    m_current_node = nullptr;
    free(m_nodes_to_be_processed);
    free(m_solution_pool);

    if (!m_active_nodes) {
        throw Exception("No node selection strategy was given.");
    }

    if (!m_branching_strategy) {
        throw Exception("No branching strategy was given.");
    }

    m_active_nodes->initialize();
}

template<class NodeT>
const NodeT &NodeStrategies::Basic<NodeT>::current_node() const {
    return *m_current_node;
}

template<class NodeT>
bool NodeStrategies::Basic<NodeT>::has_node_to_be_processed() const {
    return !m_nodes_to_be_processed.empty();
}

template<class NodeT>
void NodeStrategies::Basic<NodeT>::set_current_node_to_next_node_to_be_processed() {
    m_current_node = m_nodes_to_be_processed.back();
    m_nodes_to_be_processed.pop_back();
}

template<class NodeT>
void NodeStrategies::Basic<NodeT>::save_current_node_solution(const Algorithm & t_solution_strategy){
    m_current_node->save_solution(t_solution_strategy);
}

template<class NodeT>
bool NodeStrategies::Basic<NodeT>::has_current_node() {
    return m_current_node;
}

template<class NodeT>
void NodeStrategies::Basic<NodeT>::add_node_to_be_processed(Node *t_node) {
    m_nodes_to_be_processed.emplace_back(dynamic_cast<NodeT*>(t_node));
}

template<class NodeT>
void NodeStrategies::Basic<NodeT>::set_current_node_as_incumbent() {
    m_best_upper_bound_node = m_current_node;
}

template<class NodeT>
void NodeStrategies::Basic<NodeT>::add_current_node_to_solution_pool() {
    m_solution_pool.emplace_back(m_current_node);
}

template<class NodeT>
void NodeStrategies::Basic<NodeT>::reset_current_node() {
    m_current_node = nullptr;
}

template<class NodeT>
void NodeStrategies::Basic<NodeT>::prune_current_node() {
    delete m_current_node;
    m_current_node = nullptr;
}

template<class NodeT>
void NodeStrategies::Basic<NodeT>::add_current_node_to_active_nodes() {
    m_active_nodes->add(m_current_node);
    m_current_node = nullptr;
}

template<class NodeT>
bool NodeStrategies::Basic<NodeT>::has_incumbent() const {
    return m_best_upper_bound_node;
}

template<class NodeT>
const Node &NodeStrategies::Basic<NodeT>::incumbent() const {
    return *m_best_upper_bound_node;
}

template<class NodeT>
template<class T, class... Args>
typename T::template Strategy<NodeT> &NodeStrategies::Basic<NodeT>::set_active_node_manager_strategy(Args &&... t_args) {
    auto* active_node_manager = new typename T::template Strategy<NodeT>(std::forward<Args>(t_args)...);
    m_active_nodes.reset(active_node_manager);
    return *active_node_manager;
}

template<class NodeT>
template<class T, class... Args>
typename T::template Strategy<NodeT> &NodeStrategies::Basic<NodeT>::set_branching_strategy(Args &&... t_args) {
    auto* branching_strategy = new typename T::template Strategy<NodeT>(std::forward<Args>(t_args)...);
    m_branching_strategy.reset(branching_strategy);
    return *branching_strategy;
}

template<class NodeT>
template<class T, class... Args>
typename T::template Strategy<NodeT> &NodeStrategies::Basic<NodeT>::set_node_updator_strategy(Args &&... t_args) {
    auto* node_updator = new typename T::template Strategy<NodeT>(std::forward<Args>(t_args)...);
    m_node_updator.reset(node_updator);
    return *node_updator;
}

template<class NodeT>
bool NodeStrategies::Basic<NodeT>::current_node_has_a_valid_solution() const {
    return current_node().status() != Infeasible && m_branching_strategy->is_valid(current_node());
}

template<class NodeT>
unsigned int NodeStrategies::Basic<NodeT>::create_child_nodes() {

    const auto& selected_node = m_active_nodes->node_selected_for_branching();
    auto child_nodes = m_branching_strategy->create_child_nodes(selected_node, [&](){ return m_node_id++; });

    for (auto* node : child_nodes) {
        EASY_LOG(Trace, "branch-and-bound", "Node " << node->id() << " has been created from " << selected_node.id() << '.');
        m_nodes_to_be_processed.emplace_back(node);
    }

    return child_nodes.size();
}

template<class NodeT>
void NodeStrategies::Basic<NodeT>::create_root_node() {
    m_nodes_to_be_processed.template emplace_back(new NodeT(m_node_id++));
}

template<class NodeT>
void NodeStrategies::Basic<NodeT>::apply_current_node_to(Algorithm &t_solution_strategy) {
    m_node_updator->apply_local_changes(current_node(), t_solution_strategy);
}

#endif //OPTIMIZE_NODESTRATEGIES_BASIC_H
