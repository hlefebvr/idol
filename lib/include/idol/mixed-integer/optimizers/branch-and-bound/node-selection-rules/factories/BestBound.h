//
// Created by henri on 22/03/23.
//

#ifndef IDOL_BESTBOUND_H
#define IDOL_BESTBOUND_H

#include "NodeSelectionRuleFactory.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/impls/BestBound.h"

namespace idol {
    class BestBound;
}

class idol::BestBound {
public:
    template<class NodeT>
    class Strategy : public NodeSelectionRuleFactory<NodeT> {
    public:
        explicit Strategy(const BestBound& t_parent) {}

        NodeSelectionRules::BestBound<NodeT> *operator()(Optimizers::BranchAndBound<NodeT>& t_parent) const override {
            return new NodeSelectionRules::BestBound<NodeT>(t_parent);
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_BESTBOUND_H
