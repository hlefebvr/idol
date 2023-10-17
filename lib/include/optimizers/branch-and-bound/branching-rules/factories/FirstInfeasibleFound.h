//
// Created by henri on 17.10.23.
//

#ifndef IDOL_FIRSTINFEASIBLEFOUND_H
#define IDOL_FIRSTINFEASIBLEFOUND_H

#include "VariableBranching.h"
#include "optimizers/branch-and-bound/branching-rules/impls/FirstInfeasibleFound.h"

namespace idol {
    class FirstInfeasibleFound;
}

class idol::FirstInfeasibleFound : public idol::VariableBranching {
public:
    FirstInfeasibleFound() = default;

    template<class NodeInfoT>
    class Strategy : public VariableBranching::Strategy<NodeInfoT> {
    public:
        Strategy() = default;

        explicit Strategy(const FirstInfeasibleFound& t_parent) : VariableBranching::Strategy<NodeInfoT>(t_parent) {}

        BranchingRules::VariableBranching<NodeInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeInfoT> &t_parent) const override {
            return new BranchingRules::FirstInfeasibleFound<NodeInfoT>(t_parent, idol::VariableBranching::Strategy<NodeInfoT>::create_branching_candidates(t_parent.parent()));
        }

        VariableBranching::Strategy<NodeInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

};

#endif //IDOL_FIRSTINFEASIBLEFOUND_H
