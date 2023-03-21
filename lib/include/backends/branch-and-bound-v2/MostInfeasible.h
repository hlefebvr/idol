//
// Created by henri on 21/03/23.
//

#ifndef IDOL_MOSTINFEASIBLE_H
#define IDOL_MOSTINFEASIBLE_H

#include "BranchingRuleFactory.h"

class MostInfeasible {
public:
    template<class NodeT>
    class Strategy : public BranchingRuleFactory<NodeT> {
    public:
        explicit Strategy(const MostInfeasible& t_parent) {}

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_MOSTINFEASIBLE_H
