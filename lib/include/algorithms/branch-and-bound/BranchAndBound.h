//
// Created by henri on 13/09/22.
//

#ifndef OPTIMIZE_BRANCHANDBOUND_H
#define OPTIMIZE_BRANCHANDBOUND_H

#include "Node.h"
#include "AbstractSolutionStrategy.h"
#include "AbstractBranchingStrategy.h"
#include "AbstractNodeStrategy.h"
#include "../../modeling/numericals.h"
#include "../logs/Log.h"
#include <vector>
#include <list>
#include <memory>

template <class T, class Container = std::list<T>>
class Stack : private Container {
public:
    using std::list<T>::begin;
    using std::list<T>::end;
    using std::list<T>::rbegin;
    using std::list<T>::rend;
    using std::list<T>::cbegin;
    using std::list<T>::cend;
    using std::list<T>::crbegin;
    using std::list<T>::crend;

    void add(T&& t_object) { this->emplace_back(std::move(t_object)); }
    void add(const T& t_object) { this->emplace_back(t_object); }
    T& top() { return this->back(); }
    [[nodiscard]] const T& top() const { return this->back(); }
    void pop() { this->pop_back(); }

    explicit operator bool() const { return !this->empty(); }
};

class BranchAndBound {
    unsigned int m_n_created_nodes = 0;
    bool m_is_terminated = false;

    // Nodes
    double m_best_lower_bound = -Inf;
    double m_best_upper_bound = +Inf;
    Node* m_best_upper_bound_node = nullptr;
    Node* m_current_node = nullptr;
    std::list<Node*> m_solution_pool;
    std::vector<Node*> m_active_nodes;
    Stack<Node*> m_nodes_to_be_processed;

    // User strategies
    std::unique_ptr<AbstractSolutionStrategy> m_solution_strategy;
    std::unique_ptr<AbstractBranchingStrategy> m_branching_strategy;
    std::unique_ptr<AbstractNodeStrategy> m_node_strategy;

    void initialize();
    void create_root_node();
    void solve_queued_nodes();
    void update_current_node();
    void apply_local_changes();
    void solve_current_node();
    void analyze_current_node();
    [[nodiscard]] bool current_node_is_root_node() const;
    [[nodiscard]] bool current_node_is_infeasible() const;
    [[nodiscard]] bool current_node_is_unbounded() const;
    [[nodiscard]] bool current_node_was_not_solved_to_optimality() const;
    [[nodiscard]] bool current_node_has_a_valid_solution() const;
    void set_current_node_as_incumbent();
    void add_current_node_to_solution_pool();
    void reset_current_node();
    bool current_node_is_below_upper_bound();
    bool current_node_is_above_upper_bound();
    void apply_heuristics_on_current_node();
    void prune_current_node();
    void add_current_node_to_active_nodes();
    void reset_local_changes();
    void prune_active_nodes_by_bound();
    void update_best_lower_bound();
    bool no_active_nodes();
    void branch();

    void terminate();
    void terminate_for_no_active_nodes();
    void terminate_for_gap_is_closed();
    void terminate_for_infeasibility();
    void terminate_for_unboundedness();
    void terminate_for_node_could_not_be_solved_to_optimality();

    [[nodiscard]] bool gap_is_closed() const { return relative_gap() <= ToleranceForRelativeGapMIP || absolute_gap() <= ToleranceForAbsoluteGapMIP; }

    void log_node(LogLevel t_msg_level, const Node& t_node) const;
public:
    virtual ~BranchAndBound();

    void solve();

    [[nodiscard]] bool is_terminated() const { return m_is_terminated; }

    void set_solution_strategy(AbstractSolutionStrategy* t_node_strategy);
    void set_branching_strategy(AbstractBranchingStrategy* t_branching_strategy);
    void set_node_strategy(AbstractNodeStrategy* t_node_strategy);

    [[nodiscard]] double lower_bound() const { return m_best_lower_bound; }
    [[nodiscard]] double upper_bound() const { return m_best_upper_bound; }
    [[nodiscard]] double relative_gap() const;
    [[nodiscard]] double absolute_gap() const;

    [[nodiscard]] unsigned int n_created_nodes() const { return m_n_created_nodes; }

    [[nodiscard]] SolutionStatus status() const;
    [[nodiscard]] double objective_value() const;
    [[nodiscard]] Solution::Primal primal_solution() const;
};

#endif //OPTIMIZE_BRANCHANDBOUND_H
