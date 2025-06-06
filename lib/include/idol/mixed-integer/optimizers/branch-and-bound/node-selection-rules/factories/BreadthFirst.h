//
// Created by henri on 22/03/23.
//

#ifndef IDOL_BREADTHFIRST_H
#define IDOL_BREADTHFIRST_H

#include "NodeSelectionRuleFactory.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/impls/BreadthFirst.h"

namespace idol {
    class BreadthFirst;
}

class idol::BreadthFirst {
public:
    template<class NodeT>
    class Strategy : public NodeSelectionRuleFactory<NodeT> {
    public:
        explicit Strategy(const BreadthFirst& t_parent) {}

        NodeSelectionRules::BreadthFirst<NodeT> *operator()(Optimizers::BranchAndBound<NodeT>& t_parent) const override {
            return new NodeSelectionRules::BreadthFirst<NodeT>(t_parent);
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_BREADTHFIRST_H
