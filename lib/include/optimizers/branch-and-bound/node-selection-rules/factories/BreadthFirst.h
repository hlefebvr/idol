//
// Created by henri on 22/03/23.
//

#ifndef IDOL_BREADTHFIRST_H
#define IDOL_BREADTHFIRST_H

#include "optimizers/branch-and-bound/node-selection-rules/factories/NodeSelectionRuleFactory.h"
#include "optimizers/branch-and-bound/node-selection-rules/impls/BreadthFirst.h"

class BreadthFirst {
public:
    template<class NodeT>
    class Strategy : public NodeSelectionRuleFactory<NodeT> {
    public:
        explicit Strategy(const BreadthFirst& t_parent) {}

        NodeSelectionRules::BreadthFirst<NodeT> *operator()() const override {
            return new NodeSelectionRules::BreadthFirst<NodeT>();
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_BREADTHFIRST_H
