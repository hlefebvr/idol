//
// Created by henri on 21/03/23.
//

#ifndef IDOL_DEPTHFIRST_H
#define IDOL_DEPTHFIRST_H

#include "backends/branch-and-bound-v2/node-selection-rules/factories/NodeSelectionRuleFactory.h"
#include "backends/branch-and-bound-v2/node-selection-rules/impls/DepthFirst.h"

class DepthFirst {
public:
    template<class NodeT>
    class Strategy : public NodeSelectionRuleFactory<NodeT> {
    public:
        explicit Strategy(const DepthFirst& t_parent) {}

        NodeSelectionRules::DepthFirst<NodeT> *operator()() const override {
            return new NodeSelectionRules::DepthFirst<NodeT>();
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_DEPTHFIRST_H
