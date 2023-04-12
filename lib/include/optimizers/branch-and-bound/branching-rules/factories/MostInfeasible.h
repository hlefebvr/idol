//
// Created by henri on 21/03/23.
//

#ifndef IDOL_MOSTINFEASIBLE_H
#define IDOL_MOSTINFEASIBLE_H

#include "optimizers/branch-and-bound/branching-rules/factories/BranchingRuleFactory.h"
#include "optimizers/branch-and-bound/branching-rules/impls/MostInfeasbile.h"

class MostInfeasible {
    std::optional<std::list<Var>> m_explicit_branching_candidates;
public:
    MostInfeasible() = default;

    template<class IteratorT>
    MostInfeasible(IteratorT t_begin, IteratorT t_end) : m_explicit_branching_candidates(std::list<Var>()) {

        for ( ; t_begin != t_end ; ++t_begin) {
            m_explicit_branching_candidates.value().emplace_back(*t_begin);
        }

    }

    template<class NodeT>
    class Strategy : public BranchingRuleFactory<NodeT> {
        std::optional<std::list<Var>> m_explicit_branching_candidates;
    public:
        Strategy() = default;

        explicit Strategy(const MostInfeasible& t_parent)
            : m_explicit_branching_candidates(t_parent.m_explicit_branching_candidates) {}

        BranchingRules::MostInfeasible<NodeT> *operator()(const Model& t_model) const override {
            if (m_explicit_branching_candidates.has_value()) {
                return new BranchingRules::MostInfeasible<NodeT>(t_model, m_explicit_branching_candidates.value());
            }
            return new BranchingRules::MostInfeasible<NodeT>(t_model);
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };
};

#endif //IDOL_MOSTINFEASIBLE_H
