//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_ABSTRACTNODESTARTEGY_H
#define OPTIMIZE_ABSTRACTNODESTARTEGY_H

class AbstractNode;
class AbstractSolutionStrategy;
class AbstractActiveNodeManager;

class AbstractNodeStrategy {
public:
    virtual ~AbstractNodeStrategy() = default;
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
    [[nodiscard]] virtual bool has_incumbent() const = 0;
    [[nodiscard]] virtual const AbstractNode& incumbent() const = 0;
    [[nodiscard]] virtual bool current_node_has_a_valid_solution() const = 0;
    virtual unsigned int create_child_nodes() = 0;
    virtual void create_root_node() = 0;
    virtual void apply_current_node_to(AbstractSolutionStrategy& t_solution_strategy) = 0;

    [[nodiscard]] virtual AbstractActiveNodeManager& active_nodes() = 0;
    [[nodiscard]] virtual const AbstractActiveNodeManager& active_nodes() const = 0;
};

#endif //OPTIMIZE_ABSTRACTNODESTARTEGY_H
