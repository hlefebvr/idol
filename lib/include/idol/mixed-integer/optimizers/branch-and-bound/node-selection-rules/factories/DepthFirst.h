//
// Created by henri on 21/03/23.
//

#ifndef IDOL_DEPTHFIRST_H
#define IDOL_DEPTHFIRST_H

#include "NodeSelectionRuleFactory.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/impls/DepthFirst.h"

namespace idol {
    class DepthFirst;
}

class idol::DepthFirst {
public:
    template<class NodeT>
    class Strategy : public NodeSelectionRuleFactory<NodeT> {
    public:
        explicit Strategy(const DepthFirst& t_parent) {}

        NodeSelectionRules::DepthFirst<NodeT> *operator()(Optimizers::BranchAndBound<NodeT>& t_parent) const override {
            return new NodeSelectionRules::DepthFirst<NodeT>(t_parent);
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_DEPTHFIRST_H
