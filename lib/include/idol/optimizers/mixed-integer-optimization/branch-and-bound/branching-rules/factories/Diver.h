//
// Created by henri on 17.10.23.
//

#ifndef IDOL_DIVER_H
#define IDOL_DIVER_H

#include "VariableBranching.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/branching-rules/impls/Diver.h"

namespace idol {
    template<class BranchingRuleT> class Diver;
}

template<class BranchingRuleT>
class idol::Diver : public idol::VariableBranching {
public:
    Diver() = default;

    template<class IteratorT>
    Diver(IteratorT t_begin, IteratorT t_end) : idol::VariableBranching(t_begin, t_end) {}

    template<class NodeInfoT>
    class Strategy : public VariableBranching::Strategy<NodeInfoT> {
    public:
        Strategy() = default;

        explicit Strategy(const Diver& t_parent) : VariableBranching::Strategy<NodeInfoT>(t_parent) {}

        BranchingRules::VariableBranching<NodeInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeInfoT> &t_parent) const override {
            return new BranchingRules::Diver<BranchingRuleT>(t_parent, idol::VariableBranching::Strategy<NodeInfoT>::create_branching_candidates(t_parent.parent()));
        }

        VariableBranching::Strategy<NodeInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

};

#endif //IDOL_DIVER_H
