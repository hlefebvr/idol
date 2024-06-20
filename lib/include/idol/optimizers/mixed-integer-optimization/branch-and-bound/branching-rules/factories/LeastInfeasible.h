//
// Created by henri on 17.10.23.
//

#ifndef IDOL_LEASTINFEASIBLE_H
#define IDOL_LEASTINFEASIBLE_H

#include "VariableBranching.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/branching-rules/impls/LeastInfeasible.h"

namespace idol {
    class LeastInfeasible;
}

class idol::LeastInfeasible : public idol::VariableBranching {
public:
    LeastInfeasible() = default;

    template<class IteratorT>
    LeastInfeasible(IteratorT t_begin, IteratorT t_end) : idol::VariableBranching(t_begin, t_end) {}

    template<class NodeInfoT>
    class Strategy : public VariableBranching::Strategy<NodeInfoT> {
    public:
        Strategy() = default;

        explicit Strategy(const LeastInfeasible& t_parent) : VariableBranching::Strategy<NodeInfoT>(t_parent) {}

        BranchingRules::VariableBranching<NodeInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeInfoT> &t_parent) const override {
            return new BranchingRules::LeastInfeasible<NodeInfoT>(t_parent, idol::VariableBranching::Strategy<NodeInfoT>::create_branching_candidates(t_parent.parent()));
        }

        VariableBranching::Strategy<NodeInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

};

#endif //IDOL_LEASTINFEASIBLE_H
