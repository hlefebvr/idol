//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_BRANCHINGSTRATEGY_H
#define OPTIMIZE_BRANCHINGSTRATEGY_H

#include <list>
#include <functional>

class BranchingStrategy {
public:
    virtual ~BranchingStrategy() = default;
};

template<class NodeT>
class BranchingStrategyWithNodeType : public BranchingStrategy {
public:
    virtual bool is_valid(const NodeT& t_node) const = 0;

    virtual std::list<NodeT*> create_child_nodes(const NodeT &t_node, const std::function<unsigned int()>& t_id_provider) = 0;
};

#endif //OPTIMIZE_BRANCHINGSTRATEGY_H
