//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_BASENODESTRATEGY_H
#define OPTIMIZE_BASENODESTRATEGY_H

#include "AbstractNodeStrategy.h"
#include <cassert>
#include <vector>

class AbstractSolutionStrategy;

template<class NodeT>
class BaseNodeStrategy : public AbstractNodeStrategy {
protected:
    virtual void apply_local_changes(const NodeT& t_node, AbstractSolutionStrategy& t_solution_strategy) = 0;
public:
    [[nodiscard]] NodeT *create_root_node() const override { return new NodeT(0); }

    void prepare_node_solution(const AbstractNode& t_node, AbstractSolutionStrategy& t_solution_strategy) override {
        // The cast could be avoided by introducing indices for nodes inside a vector of NodeT*, each one
        // notifying for their death, as done for modeling objects inside a model.
        // However, I find it unworthy. This would result in additional space usage while having very
        // minor impact on code clarity or elegance.
        auto* casted_node = dynamic_cast<const NodeT*>(&t_node);
        assert(casted_node);
        apply_local_changes(*casted_node, t_solution_strategy);
    }
};

#endif //OPTIMIZE_BASENODESTRATEGY_H
