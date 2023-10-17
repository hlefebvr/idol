//
// Created by henri on 17.10.23.
//

#ifndef IDOL_MOSTINFEASIBLE_H
#define IDOL_MOSTINFEASIBLE_H

#include "VariableBranching.h"
#include "optimizers/branch-and-bound/branching-rules/impls/MostInfeasible.h"

namespace idol {
    class MostInfeasible;
}

class idol::MostInfeasible : public idol::VariableBranching {
public:
    MostInfeasible() = default;

    template<class NodeInfoT>
    class Strategy : public VariableBranching::Strategy<NodeInfoT> {
    public:
        Strategy() = default;

        explicit Strategy(const MostInfeasible& t_parent) : VariableBranching::Strategy<NodeInfoT>(t_parent) {}

        BranchingRules::VariableBranching<NodeInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeInfoT> &t_parent) const override {
            return new BranchingRules::MostInfeasible<NodeInfoT>(t_parent, idol::VariableBranching::Strategy<NodeInfoT>::create_branching_candidates(t_parent.parent()));
        }

        VariableBranching::Strategy<NodeInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

};

#endif //IDOL_MOSTINFEASIBLE_H
