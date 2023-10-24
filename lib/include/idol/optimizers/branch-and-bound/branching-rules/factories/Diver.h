//
// Created by henri on 17.10.23.
//

#ifndef IDOL_DIVER_H
#define IDOL_DIVER_H

#include "VariableBranching.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/Diver.h"

namespace idol {
    template<class BranchingRuleT> class Diver;
}

template<class BranchingRuleT>
class idol::Diver : public idol::VariableBranching {
public:
    Diver() = default;

    template<class IteratorT>
    Diver(IteratorT t_begin, IteratorT t_end) : idol::VariableBranching(t_begin, t_end) {}

    template<class NodeVarInfoT>
    class Strategy : public VariableBranching::Strategy<NodeVarInfoT> {
    public:
        Strategy() = default;

        explicit Strategy(const Diver& t_parent) : VariableBranching::Strategy<NodeVarInfoT>(t_parent) {}

        BranchingRules::VariableBranching<NodeVarInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeVarInfoT> &t_parent) const override {
            return new BranchingRules::Diver<BranchingRuleT>(t_parent, idol::VariableBranching::Strategy<NodeVarInfoT>::create_branching_candidates(t_parent.parent()));
        }

        VariableBranching::Strategy<NodeVarInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

};

#endif //IDOL_DIVER_H
