//
// Created by henri on 22/03/23.
//

#ifndef IDOL_WORSTBOUND_H
#define IDOL_WORSTBOUND_H

#include "NodeSelectionRuleFactory.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/node-selection-rules/impls/WorstBound.h"

namespace idol {
    class WorstBound;
}

class idol::WorstBound {
public:
    template<class NodeT>
    class Strategy : public NodeSelectionRuleFactory<NodeT> {
    public:
        explicit Strategy(const WorstBound& t_parent) {}

        NodeSelectionRules::WorstBound<NodeT> *operator()(Optimizers::BranchAndBound<NodeT>& t_parent) const override {
            return new NodeSelectionRules::WorstBound<NodeT>(t_parent);
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_WORSTBOUND_H
