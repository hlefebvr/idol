//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_NODESTRATEGY_H
#define OPTIMIZE_NODESTRATEGY_H

#include "AbstractNodeStrategy.h"

template<class NodeT>
class NodeStrategy : public AbstractNodeStrategy {
public:
    [[nodiscard]] Node *create_root_node() const override { return new NodeT(0); }
};

#endif //OPTIMIZE_NODESTRATEGY_H
