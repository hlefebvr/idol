//
// Created by henri on 21/03/23.
//

#ifndef IDOL_MOSTINFEASIBLE_H
#define IDOL_MOSTINFEASIBLE_H

#include "optimizers/branch-and-bound/branching-rules/factories/BranchingRuleFactory.h"
#include "optimizers/branch-and-bound/branching-rules/impls/MostInfeasbile.h"

class MostInfeasible {
public:
    template<class NodeT>
    class Strategy : public BranchingRuleFactory<NodeT> {
    public:
        explicit Strategy(const MostInfeasible& t_parent) {}

        BranchingRules::MostInfeasible<NodeT> *operator()(const Model& t_model) const override {
            return new BranchingRules::MostInfeasible<NodeT>(t_model);
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_MOSTINFEASIBLE_H
