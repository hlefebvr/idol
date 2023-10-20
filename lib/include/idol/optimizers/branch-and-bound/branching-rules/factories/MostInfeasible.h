//
// Created by henri on 17.10.23.
//

#ifndef IDOL_MOSTINFEASIBLE_H
#define IDOL_MOSTINFEASIBLE_H

#include "VariableBranching.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/MostInfeasible.h"

namespace idol {
    class MostInfeasible;
}

class idol::MostInfeasible : public idol::VariableBranching {
public:
    MostInfeasible() = default;

    template<class IteratorT>
    MostInfeasible(IteratorT t_begin, IteratorT t_end) : idol::VariableBranching(t_begin, t_end) {}

    template<class NodeVarInfoT>
    class Strategy : public VariableBranching::Strategy<NodeVarInfoT> {
    public:
        Strategy() = default;

        explicit Strategy(const MostInfeasible& t_parent) : VariableBranching::Strategy<NodeVarInfoT>(t_parent) {}

        BranchingRules::VariableBranching<NodeVarInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeVarInfoT> &t_parent) const override {
            return new BranchingRules::MostInfeasible<NodeVarInfoT>(t_parent, idol::VariableBranching::Strategy<NodeVarInfoT>::create_branching_candidates(t_parent.parent()));
        }

        VariableBranching::Strategy<NodeVarInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

};

#endif //IDOL_MOSTINFEASIBLE_H
