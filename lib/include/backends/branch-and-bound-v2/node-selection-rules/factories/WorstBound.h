//
// Created by henri on 22/03/23.
//

#ifndef IDOL_WORSTBOUND_H
#define IDOL_WORSTBOUND_H

#include "backends/branch-and-bound-v2/node-selection-rules/factories/NodeSelectionRuleFactory.h"
#include "backends/branch-and-bound-v2/node-selection-rules/impls/WorstBound.h"

class WorstBound {
public:
    template<class NodeT>
    class Strategy : public NodeSelectionRuleFactory<NodeT> {
    public:
        explicit Strategy(const WorstBound& t_parent) {}

        NodeSelectionRules::WorstBound<NodeT> *operator()() const override {
            return new NodeSelectionRules::WorstBound<NodeT>();
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_WORSTBOUND_H
