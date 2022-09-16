//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_ABSTRACTBRANCHINGSTRATEGY_H
#define OPTIMIZE_ABSTRACTBRANCHINGSTRATEGY_H

#include <list>

class AbstractNode;

class AbstractBranchingStrategy {
public:
    virtual ~AbstractBranchingStrategy() = default;

    [[nodiscard]] virtual bool is_valid(const AbstractNode& t_node) const = 0;

    virtual std::list<AbstractNode*> create_child_nodes(unsigned int t_id, const AbstractNode& t_node) = 0;
};

#endif //OPTIMIZE_ABSTRACTBRANCHINGSTRATEGY_H
