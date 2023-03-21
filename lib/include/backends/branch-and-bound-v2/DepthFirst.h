//
// Created by henri on 21/03/23.
//

#ifndef IDOL_DEPTHFIRST_H
#define IDOL_DEPTHFIRST_H

#include "NodeSelectionRuleFactory.h"

class DepthFirst {
public:
    template<class NodeT>
    class Strategy : public NodeSelectionRuleFactory<NodeT> {
    public:
        explicit Strategy(const DepthFirst& t_parent) {}

        NodeSelectionRuleFactory<NodeT> *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_DEPTHFIRST_H
