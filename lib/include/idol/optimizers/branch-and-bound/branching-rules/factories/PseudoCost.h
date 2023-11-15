//
// Created by henri on 18.10.23.
//

#ifndef IDOL_PSEUDOCOST_H
#define IDOL_PSEUDOCOST_H

#include "VariableBranching.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/PseudoCost.h"

namespace idol {
    class PseudoCost;
}

class idol::PseudoCost : public idol::VariableBranching {
public:
    PseudoCost() = default;

    template<class IteratorT>
    PseudoCost(IteratorT t_begin, IteratorT t_end) : idol::VariableBranching(t_begin, t_end) {}

    template<class NodeInfoT>
    class Strategy : public VariableBranching::Strategy<NodeInfoT> {
    public:
        Strategy() = default;

        explicit Strategy(const PseudoCost& t_parent) : VariableBranching::Strategy<NodeInfoT>(t_parent) {}

        BranchingRules::VariableBranching<NodeInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeInfoT> &t_parent) const override {
            return new BranchingRules::PseudoCost<NodeInfoT>(t_parent, idol::VariableBranching::Strategy<NodeInfoT>::create_branching_candidates(t_parent.parent()));
        }

        VariableBranching::Strategy<NodeInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

};
#endif //IDOL_PSEUDOCOST_H
