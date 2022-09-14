//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_ABSTRACTNODESTRATEGY_H
#define OPTIMIZE_ABSTRACTNODESTRATEGY_H

class Node;

class AbstractNodeStrategy {
public:
    virtual ~AbstractNodeStrategy() = default;

    [[nodiscard]] virtual Node* create_root_node(unsigned int t_id) const = 0;
};

#endif //OPTIMIZE_ABSTRACTNODESTRATEGY_H
