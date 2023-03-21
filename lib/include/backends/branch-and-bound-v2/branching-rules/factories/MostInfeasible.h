//
// Created by henri on 21/03/23.
//

#ifndef IDOL_MOSTINFEASIBLE_H
#define IDOL_MOSTINFEASIBLE_H

#include "backends/branch-and-bound-v2/branching-rules/factories/BranchingRuleFactory.h"
#include "backends/branch-and-bound-v2/branching-rules/impls/MostInfeasbile.h"

class MostInfeasible {
public:
    template<class NodeT>
    class Strategy : public BranchingRuleFactory<NodeT> {
    public:
        explicit Strategy(const MostInfeasible& t_parent) {}

        BranchingRules::MostInfeasible<NodeT> *operator()(const AbstractModel& t_model) const override {
            return new BranchingRules::MostInfeasible<NodeT>(t_model);
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_MOSTINFEASIBLE_H
