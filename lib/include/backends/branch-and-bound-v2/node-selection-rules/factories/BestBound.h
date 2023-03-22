//
// Created by henri on 22/03/23.
//

#ifndef IDOL_BESTBOUND_H
#define IDOL_BESTBOUND_H

#include "backends/branch-and-bound-v2/node-selection-rules/factories/NodeSelectionRuleFactory.h"
#include "backends/branch-and-bound-v2/node-selection-rules/impls/BestBound.h"

class BestBound {
public:
    template<class NodeT>
    class Strategy : public NodeSelectionRuleFactory<NodeT> {
    public:
        explicit Strategy(const BestBound& t_parent) {}

        NodeSelectionRules::BestBound<NodeT> *operator()() const override {
            return new NodeSelectionRules::BestBound<NodeT>();
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_BESTBOUND_H
