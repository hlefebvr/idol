//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_ABSTRACTBRANCHINGSTRATEGY_H
#define OPTIMIZE_ABSTRACTBRANCHINGSTRATEGY_H

class AbstractBranchingStrategy {
public:
    virtual ~AbstractBranchingStrategy() = default;
};

template<class NodeT>
class AbstractBranchingStrategyWithType : public AbstractBranchingStrategy {
public:
    virtual bool is_valid(const NodeT& t_node) const = 0;

    virtual std::list<NodeT*> create_child_nodes(const NodeT &t_node, const std::function<unsigned int()>& t_id_provider) const = 0;
};

#endif //OPTIMIZE_ABSTRACTBRANCHINGSTRATEGY_H
