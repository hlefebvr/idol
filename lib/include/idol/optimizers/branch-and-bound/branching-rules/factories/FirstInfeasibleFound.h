//
// Created by henri on 17.10.23.
//

#ifndef IDOL_FIRSTINFEASIBLEFOUND_H
#define IDOL_FIRSTINFEASIBLEFOUND_H

#include "VariableBranching.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/FirstInfeasibleFound.h"

namespace idol {
    class FirstInfeasibleFound;
}

class idol::FirstInfeasibleFound : public idol::VariableBranching {
public:
    FirstInfeasibleFound() = default;

    template<class NodeVarInfoT>
    class Strategy : public VariableBranching::Strategy<NodeVarInfoT> {
    public:
        Strategy() = default;

        explicit Strategy(const FirstInfeasibleFound& t_parent) : VariableBranching::Strategy<NodeVarInfoT>(t_parent) {}

        BranchingRules::VariableBranching<NodeVarInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeVarInfoT> &t_parent) const override {
            return new BranchingRules::FirstInfeasibleFound<NodeVarInfoT>(t_parent, idol::VariableBranching::Strategy<NodeVarInfoT>::create_branching_candidates(t_parent.parent()));
        }

        VariableBranching::Strategy<NodeVarInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

};

#endif //IDOL_FIRSTINFEASIBLEFOUND_H
