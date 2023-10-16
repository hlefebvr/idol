//
// Created by henri on 16.10.23.
//

#ifndef IDOL_VARIABLEBRANCHING_H
#define IDOL_VARIABLEBRANCHING_H

#include <optional>
#include <list>
#include "optimizers/branch-and-bound/branching-rules/factories/BranchingRuleFactory.h"
#include "optimizers/branch-and-bound/branching-rules/impls/VariableBranching.h"
#include "modeling/models/Model.h"
#include "optimizers/branch-and-bound/scoring-functions/VariableScoringFunctionFactory.h"

namespace idol {
    class VariableBranching;
}

class idol::VariableBranching {
    std::optional<std::list<Var>> m_explicit_branching_candidates;
    std::unique_ptr<VariableScoringFunctionFactory> m_scoring_function_factory;
public:
    VariableBranching() = default;

    VariableBranching(const VariableScoringFunctionFactory& t_scoring_function)
        : m_scoring_function_factory(t_scoring_function.clone()) {}

    template<class IteratorT>
    VariableBranching(IteratorT t_begin, IteratorT t_end) : m_explicit_branching_candidates(std::list<Var>()) {

        for ( ; t_begin != t_end ; ++t_begin) {
            m_explicit_branching_candidates.value().emplace_back(*t_begin);
        }

    }

    VariableBranching& with_scoring_function(const VariableScoringFunctionFactory& t_scoring_function) {
        m_scoring_function_factory.reset(t_scoring_function.clone());
        return *this;
    }

    template<class NodeT>
    class Strategy : public BranchingRuleFactory<NodeT> {

        std::optional<std::list<Var>> m_explicit_branching_candidates;
        std::unique_ptr<BranchingRuleFactory<NodeT>> m_then;
        std::unique_ptr<VariableScoringFunctionFactory> m_scoring_function_factory;

        Strategy(const Strategy& t_src)
            : m_explicit_branching_candidates(t_src.m_explicit_branching_candidates),
              m_then(t_src.m_then ? t_src.m_then->clone() : nullptr),
              m_scoring_function_factory(t_src.m_scoring_function_factory ? t_src.m_scoring_function_factory->clone() : nullptr) {}

        static std::list<Var> create_default_branching_candidates(const Model& t_model);
    public:
        Strategy() = default;

        template<class IteratorT>
        Strategy(IteratorT t_begin, IteratorT t_end) : m_explicit_branching_candidates(std::list<Var>()) {

            for ( ; t_begin != t_end ; ++t_begin) {
                m_explicit_branching_candidates.value().emplace_back(*t_begin);
            }

        }

        explicit Strategy(const VariableBranching& t_parent)
                : m_explicit_branching_candidates(t_parent.m_explicit_branching_candidates),
                  m_scoring_function_factory(t_parent.m_scoring_function_factory ? t_parent.m_scoring_function_factory->clone() : nullptr) {}

        BranchingRules::VariableBranching<NodeT> *operator()(const Model& t_model) const override {

            if (!m_scoring_function_factory) {
                throw Exception("No scoring function has been given.");
            }

            return new BranchingRules::VariableBranching<NodeT>(t_model,
                                                                m_explicit_branching_candidates.has_value() ? m_explicit_branching_candidates.value() : create_default_branching_candidates(t_model),
                                                                m_then ? (*m_then)(t_model) : nullptr,
                                                                (*m_scoring_function_factory)()
                                                                );
        }

        Strategy& with_scoring_function(const VariableScoringFunctionFactory& t_scoring_function) {
            m_scoring_function_factory.reset(t_scoring_function.clone());
            return *this;
        }

        Strategy& then(const BranchingRuleFactory<NodeT>& t_then) {
            if (m_then) {
                throw Exception("A subsequent branching rule has already been given.");
            }
            m_then.reset(t_then.clone());
            return *this;
        }

        Strategy *clone() const override {
            return new Strategy(*this);
        }
    };

};

template<class NodeT>
std::list<idol::Var>
idol::VariableBranching::Strategy<NodeT>::create_default_branching_candidates(const idol::Model &t_model) {

    std::list<Var> result;

    for (const auto& var : t_model.vars()) {
        if (t_model.get_var_type(var) != Continuous) {
            result.emplace_back(var);
        }
    }

    return result;
}

#endif //IDOL_VARIABLEBRANCHING_H
