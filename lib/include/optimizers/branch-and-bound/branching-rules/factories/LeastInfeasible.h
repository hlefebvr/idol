//
// Created by henri on 17.10.23.
//

#ifndef IDOL_LEASTINFEASIBLE_H
#define IDOL_LEASTINFEASIBLE_H

#include "VariableBranching.h"
#include "optimizers/branch-and-bound/branching-rules/impls/LeastInfeasible.h"

namespace idol {
    class LeastInfeasible;
}

class idol::LeastInfeasible : public idol::VariableBranching {
public:
    LeastInfeasible() = default;

    template<class NodeVarInfoT>
    class Strategy : public VariableBranching::Strategy<NodeVarInfoT> {
    public:
        Strategy() = default;

        explicit Strategy(const LeastInfeasible& t_parent) : VariableBranching::Strategy<NodeVarInfoT>(t_parent) {}

        BranchingRules::VariableBranching<NodeVarInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeVarInfoT> &t_parent) const override {
            return new BranchingRules::LeastInfeasible<NodeVarInfoT>(t_parent, idol::VariableBranching::Strategy<NodeVarInfoT>::create_branching_candidates(t_parent.parent()));
        }

        VariableBranching::Strategy<NodeVarInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

};

#endif //IDOL_LEASTINFEASIBLE_H
