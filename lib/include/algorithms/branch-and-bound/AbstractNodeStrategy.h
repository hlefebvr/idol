//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_ABSTRACTNODESTRATEGY_H
#define OPTIMIZE_ABSTRACTNODESTRATEGY_H

class AbstractNode;
class AbstractSolutionStrategy;

class AbstractNodeStrategy {
public:
    virtual ~AbstractNodeStrategy() = default;

    [[nodiscard]] virtual AbstractNode* create_root_node() const = 0;

    virtual void prepare_node_solution(const AbstractNode& t_node, AbstractSolutionStrategy& t_solution_strategy) = 0;
};

#endif //OPTIMIZE_ABSTRACTNODESTRATEGY_H
