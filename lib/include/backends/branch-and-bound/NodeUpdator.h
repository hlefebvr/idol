//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_NODEUPDATOR_H
#define OPTIMIZE_NODEUPDATOR_H

class Algorithm;

class NodeUpdator {
public:
    virtual ~NodeUpdator() = default;
};

template<class NodeT>
class NodeUpdatorWithNodeType {
public:
    virtual ~NodeUpdatorWithNodeType() = default;
    virtual void apply_local_changes(const NodeT &t_node, AbstractModel &t_solution_strategy) = 0;
};

#endif //OPTIMIZE_NODEUPDATOR_H
