//
// Created by henri on 22/03/23.
//

#ifndef IDOL_BESTESTIMATE_H
#define IDOL_BESTESTIMATE_H

#include "NodeSelectionRuleFactory.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/impls/BestEstimate.h"

namespace idol {
    class BestEstimate;
}

class idol::BestEstimate {
public:
    template<class NodeT>
    class Strategy : public NodeSelectionRuleFactory<NodeT> {
    public:
        explicit Strategy(const BestEstimate& t_parent) {}

        NodeSelectionRules::BestEstimate<NodeT> *operator()(Optimizers::BranchAndBound<NodeT>& t_parent) const override {
            return new NodeSelectionRules::BestEstimate<NodeT>(t_parent);
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_BESTESTIMATE_H
