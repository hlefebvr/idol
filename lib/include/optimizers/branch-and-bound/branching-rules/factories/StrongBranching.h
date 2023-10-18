//
// Created by henri on 17.10.23.
//

#ifndef IDOL_STRONGBRANCHING_H
#define IDOL_STRONGBRANCHING_H

#include "VariableBranching.h"
#include "optimizers/branch-and-bound/branching-rules/impls/strong-branching/NodeScoreFunction.h"
#include "optimizers/branch-and-bound/branching-rules/impls/strong-branching/StrongBranchingPhase.h"
#include "optimizers/branch-and-bound/branching-rules/impls/StrongBranching.h"

namespace idol {
    class StrongBranching;
}

class idol::StrongBranching : public idol::VariableBranching {
public:
    StrongBranching() = default;

    template<class NodeInfoT>
    class Strategy : public VariableBranching::Strategy<NodeInfoT> {
        std::optional<unsigned int> m_max_n_variables;
        std::unique_ptr<StrongBranchingScoreFunction> m_node_scoring_function;
        std::list<StrongBranchingPhase> m_phases;

        Strategy(const Strategy<NodeInfoT>& t_src);
    public:
        Strategy() = default;

        explicit Strategy(const StrongBranching& t_parent) : VariableBranching::Strategy<NodeInfoT>(t_parent) {}

        BranchingRules::VariableBranching<NodeInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeInfoT> &t_parent) const override {
            return new BranchingRules::StrongBranching<NodeInfoT>(
                        t_parent,
                        idol::VariableBranching::Strategy<NodeInfoT>::create_branching_candidates(t_parent.parent()),
                        m_max_n_variables.has_value() ? m_max_n_variables.value() : 100,
                        m_node_scoring_function ? m_node_scoring_function->clone() : new StrongBranchingScores::Product(),
                        m_phases
                    );
        }

        VariableBranching::Strategy<NodeInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

    StrongBranching& with_max_n_variables(unsigned int t_n_variables);

    StrongBranching& with_node_scoring_function(const StrongBranchingScoreFunction& t_score_function);

    StrongBranching& with_phase(const StrongBranchingPhaseType& t_phase, unsigned int t_max_n_variables, unsigned int t_max_depth);
private:
    std::optional<unsigned int> m_max_n_variables;
    std::unique_ptr<StrongBranchingScoreFunction> m_node_scoring_function;
    std::list<StrongBranchingPhase> m_phases;
};

template<class NodeInfoT>
idol::StrongBranching::Strategy<NodeInfoT>::Strategy(const Strategy<NodeInfoT>& t_src)
    : m_max_n_variables(t_src.m_max_n_variables),
      m_node_scoring_function(t_src.m_node_scoring_function ? t_src.m_node_scoring_function->clone() : nullptr),
      m_phases(t_src.m_phases)
{}

#endif //IDOL_STRONGBRANCHING_H
