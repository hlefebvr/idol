//
// Created by henri on 16.10.23.
//

#ifndef IDOL_VARIABLEBRANCHING_H
#define IDOL_VARIABLEBRANCHING_H

#include <optional>
#include <list>
#include "BranchingRuleFactory.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/VariableBranching.h"
#include "idol/modeling/models/Model.h"

namespace idol {
    class VariableBranching;
}

class idol::VariableBranching {
    std::optional<std::list<Var>> m_explicit_branching_candidates;
public:
    VariableBranching() = default;

    template<class IteratorT>
    VariableBranching(IteratorT t_begin, IteratorT t_end) : m_explicit_branching_candidates(std::list<Var>()) {

        for ( ; t_begin != t_end ; ++t_begin) {
            m_explicit_branching_candidates.value().emplace_back(*t_begin);
        }

    }

    template<class NodeT>
    class Strategy : public BranchingRuleFactory<NodeT> {
        std::optional<std::list<Var>> m_explicit_branching_candidates;
    protected:
        Strategy(const Strategy& t_src)
            : m_explicit_branching_candidates(t_src.m_explicit_branching_candidates) {}

        [[nodiscard]] std::list<Var> create_branching_candidates(const idol::Model &t_model) const;
    public:
        Strategy() = default;

        template<class IteratorT>
        Strategy(IteratorT t_begin, IteratorT t_end) : m_explicit_branching_candidates(std::list<Var>()) {

            for ( ; t_begin != t_end ; ++t_begin) {
                m_explicit_branching_candidates.value().emplace_back(*t_begin);
            }

        }

        explicit Strategy(const VariableBranching& t_parent) : m_explicit_branching_candidates(t_parent.m_explicit_branching_candidates) {}
    };

};

template<class NodeT>
std::list<idol::Var>
idol::VariableBranching::Strategy<NodeT>::create_branching_candidates(const idol::Model &t_model) const {

    if (m_explicit_branching_candidates.has_value()) {
        return m_explicit_branching_candidates.value();
    }

    std::list<Var> result;

    for (const auto& var : t_model.vars()) {
        if (t_model.get_var_type(var) != Continuous) {
            result.emplace_back(var);
        }
    }

    return result;
}

#endif //IDOL_VARIABLEBRANCHING_H
