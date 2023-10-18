//
// Created by henri on 18.10.23.
//

#ifndef IDOL_PSEUDOCOST_H
#define IDOL_PSEUDOCOST_H

#include "VariableBranching.h"
#include "optimizers/branch-and-bound/branching-rules/impls/PseudoCost.h"

namespace idol {
    class PseudoCost;
}

class idol::PseudoCost : public idol::VariableBranching {
public:
    PseudoCost() = default;

    template<class NodeVarInfoT>
    class Strategy : public VariableBranching::Strategy<NodeVarInfoT> {
    public:
        Strategy() = default;

        explicit Strategy(const PseudoCost& t_parent) : VariableBranching::Strategy<NodeVarInfoT>(t_parent) {}

        BranchingRules::VariableBranching<NodeVarInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeVarInfoT> &t_parent) const override {
            return new BranchingRules::PseudoCost<NodeVarInfoT>(t_parent, idol::VariableBranching::Strategy<NodeVarInfoT>::create_branching_candidates(t_parent.parent()));
        }

        VariableBranching::Strategy<NodeVarInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

};
#endif //IDOL_PSEUDOCOST_H
