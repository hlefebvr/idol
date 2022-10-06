//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_ABSTRACTNODEUPDATORSTRATEGY_H
#define OPTIMIZE_ABSTRACTNODEUPDATORSTRATEGY_H

class Algorithm;

template<class NodeT>
class AbstractNodeUpdatorStrategyWithType {
public:
    virtual ~AbstractNodeUpdatorStrategyWithType() = default;

    virtual void apply_local_changes(const NodeT &t_node, Algorithm &t_solution_strategy) = 0;
};

#endif //OPTIMIZE_ABSTRACTNODEUPDATORSTRATEGY_H
