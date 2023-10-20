//
// Created by henri on 17.10.23.
//

#ifndef IDOL_UNIFORMLYRANDOM_H
#define IDOL_UNIFORMLYRANDOM_H

#include "VariableBranching.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/UniformlyRandom.h"

namespace idol {
    class UniformlyRandom;
}

class idol::UniformlyRandom : public idol::VariableBranching {
    std::optional<unsigned int> m_seed;
public:
    UniformlyRandom() = default;

    template<class IteratorT>
    UniformlyRandom(IteratorT t_begin, IteratorT t_end) : idol::VariableBranching(t_begin, t_end) {}

    template<class NodeVarInfoT>
    class Strategy : public VariableBranching::Strategy<NodeVarInfoT> {
        std::optional<unsigned int> m_seed;
    public:
        Strategy() = default;

        explicit Strategy(const UniformlyRandom& t_parent)
            : VariableBranching::Strategy<NodeVarInfoT>(t_parent),
              m_seed(t_parent.m_seed) {}

        BranchingRules::VariableBranching<NodeVarInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeVarInfoT> &t_parent) const override {

            unsigned int seed = m_seed.has_value() ? m_seed.value() : (std::random_device())();

            return new BranchingRules::UniformlyRandom<NodeVarInfoT>(t_parent,
                                                                  idol::VariableBranching::Strategy<NodeVarInfoT>::create_branching_candidates(t_parent.parent()),
                                                                  seed);
        }

        VariableBranching::Strategy<NodeVarInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

    UniformlyRandom& with_seed(unsigned int t_seed) {

        if (m_seed.has_value()) {
            throw Exception("A seed has already been configured.");
        }

        m_seed = t_seed;
    }

};

#endif //IDOL_UNIFORMLYRANDOM_H
