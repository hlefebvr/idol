//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_NODESTRATEGY_H
#define OPTIMIZE_NODESTRATEGY_H

class Node;
class Algorithm;
class ActiveNodesManager;

class NodeStrategy {
public:
    virtual ~NodeStrategy() = default;
    virtual void initialize() = 0;
    virtual bool has_current_node() = 0;
    [[nodiscard]] virtual const Node& current_node() const = 0;
    [[nodiscard]] virtual bool has_node_to_be_processed() const = 0;
    virtual void set_current_node_to_next_node_to_be_processed() = 0;
    virtual void save_current_node_solution(const Algorithm& t_solution_strategy) = 0;
    virtual void add_node_to_be_processed(Node* t_node) = 0;
    virtual void set_current_node_as_incumbent() = 0;
    virtual void add_current_node_to_solution_pool() = 0;
    virtual void reset_current_node() = 0;
    virtual void prune_current_node() = 0;
    virtual void add_current_node_to_active_nodes() = 0;
    [[nodiscard]] virtual bool has_incumbent() const = 0;
    [[nodiscard]] virtual const Node& incumbent() const = 0;
    [[nodiscard]] virtual bool current_node_has_a_valid_solution() const = 0;
    virtual unsigned int create_child_nodes() = 0;
    virtual void create_root_node() = 0;
    virtual void apply_current_node_to(Algorithm& t_solution_strategy) = 0;
    virtual bool submit_solution(Solution::Primal&& t_solution, double t_best_upper_bound) = 0;
    virtual unsigned int size() const = 0;

    [[nodiscard]] virtual ActiveNodesManager& active_nodes() = 0;
    [[nodiscard]] virtual const ActiveNodesManager& active_nodes() const = 0;
};

#endif //OPTIMIZE_NODESTRATEGY_H
