//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_ABSTRACTNODESTORAGESTARTEGY_H
#define OPTIMIZE_ABSTRACTNODESTORAGESTARTEGY_H

class AbstractNode;
class AbstractSolutionStrategy;

class AbstractNodeStorageStrategy {
public:
    virtual ~AbstractNodeStorageStrategy() = default;
    virtual void initialize() = 0;
    virtual bool has_current_node() = 0;
    [[nodiscard]] virtual const AbstractNode& current_node() const = 0;
    [[nodiscard]] virtual bool has_node_to_be_processed() const = 0;
    virtual void set_current_node_to_next_node_to_be_processed() = 0;
    virtual void save_current_node_solution(const AbstractSolutionStrategy& t_solution_strategy) = 0;
    virtual void add_node_to_be_processed(AbstractNode* t_node) = 0;
    virtual void set_current_node_as_incumbent() = 0;
    virtual void add_current_node_to_solution_pool() = 0;
    virtual void reset_current_node() = 0;
    virtual void prune_current_node() = 0;
    virtual void add_current_node_to_active_nodes() = 0;
    virtual void prune_active_nodes_by_bound(double t_upper_bound) = 0;
    virtual bool has_no_active_nodes() = 0;
    [[nodiscard]] virtual const AbstractNode& lowest_node() = 0;
    [[nodiscard]] virtual const AbstractNode& select_node_for_branching() = 0;
    [[nodiscard]] virtual bool has_incumbent() const = 0;
    [[nodiscard]] virtual const AbstractNode& incumbent() const = 0;

};

#endif //OPTIMIZE_ABSTRACTNODESTORAGESTARTEGY_H
