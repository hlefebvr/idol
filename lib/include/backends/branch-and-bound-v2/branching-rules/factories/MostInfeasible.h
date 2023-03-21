//
// Created by henri on 21/03/23.
//

#ifndef IDOL_MOSTINFEASIBLE_H
#define IDOL_MOSTINFEASIBLE_H

#include "backends/branch-and-bound-v2/branching-rules/factories/BranchingRuleFactory.h"

class MostInfeasible {
public:
    template<class NodeT>
    class Strategy : public BranchingRuleFactory<NodeT> {
    public:
        explicit Strategy(const MostInfeasible& t_parent) {}

        BranchingRule<NodeT> *operator()() const override {
            std::cout << "MostInfeasible did not create anything" << std::endl;
            return nullptr;
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_MOSTINFEASIBLE_H
