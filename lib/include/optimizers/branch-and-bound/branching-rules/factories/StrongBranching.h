//
// Created by henri on 17.10.23.
//

#ifndef IDOL_STRONGBRANCHING_H
#define IDOL_STRONGBRANCHING_H

#include "VariableBranching.h"
#include "optimizers/branch-and-bound/branching-rules/impls/StrongBranching.h"

namespace idol {
    class StrongBranching;
}

class idol::StrongBranching : public idol::VariableBranching {
public:
    StrongBranching() = default;

    template<class NodeInfoT>
    class Strategy : public VariableBranching::Strategy<NodeInfoT> {
    public:
        Strategy() = default;

        explicit Strategy(const StrongBranching& t_parent) : VariableBranching::Strategy<NodeInfoT>(t_parent) {}

        BranchingRules::VariableBranching<NodeInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeInfoT> &t_parent) const override {
            return new BranchingRules::StrongBranching<NodeInfoT>(t_parent, idol::VariableBranching::Strategy<NodeInfoT>::create_branching_candidates(t_parent.parent()));
        }

        VariableBranching::Strategy<NodeInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

};

#endif //IDOL_STRONGBRANCHING_H
