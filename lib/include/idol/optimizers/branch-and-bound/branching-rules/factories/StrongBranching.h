//
// Created by henri on 17.10.23.
//

#ifndef IDOL_STRONGBRANCHING_H
#define IDOL_STRONGBRANCHING_H

#include "VariableBranching.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/NodeScoreFunction.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/strong-branching/StrongBranchingPhase.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/StrongBranching.h"

namespace idol {
    class StrongBranching;
}

class idol::StrongBranching : public idol::VariableBranching {
public:
    StrongBranching() = default;

    template<class NodeVarInfoT>
    class Strategy : public VariableBranching::Strategy<NodeVarInfoT> {
        std::optional<unsigned int> m_max_n_variables;
        std::unique_ptr<NodeScoreFunction> m_node_scoring_function;
        std::list<StrongBranchingPhase> m_phases;

        Strategy(const Strategy<NodeVarInfoT>& t_src);
    public:
        Strategy() = default;

        explicit Strategy(const StrongBranching& t_parent) : VariableBranching::Strategy<NodeVarInfoT>(t_parent) {}

        BranchingRules::VariableBranching<NodeVarInfoT> *
        operator()(const Optimizers::BranchAndBound<NodeVarInfoT> &t_parent) const override {
            return new BranchingRules::StrongBranching<NodeVarInfoT>(
                        t_parent,
                        idol::VariableBranching::Strategy<NodeVarInfoT>::create_branching_candidates(t_parent.parent()),
                        m_max_n_variables.has_value() ? m_max_n_variables.value() : 100,
                        m_node_scoring_function ? m_node_scoring_function->clone() : new NodeScoreFunctions::Product(),
                        m_phases
                    );
        }

        VariableBranching::Strategy<NodeVarInfoT> *clone() const override {
            return new Strategy(*this);
        }
    };

    StrongBranching& with_max_n_variables(unsigned int t_n_variables);

    StrongBranching& with_node_scoring_function(const NodeScoreFunction& t_score_function);

    StrongBranching& with_phase(const StrongBranchingPhaseType& t_phase, unsigned int t_max_n_variables, unsigned int t_max_depth);
private:
    std::optional<unsigned int> m_max_n_variables;
    std::unique_ptr<NodeScoreFunction> m_node_scoring_function;
    std::list<StrongBranchingPhase> m_phases;
};

template<class NodeVarInfoT>
idol::StrongBranching::Strategy<NodeVarInfoT>::Strategy(const Strategy<NodeVarInfoT>& t_src)
    : m_max_n_variables(t_src.m_max_n_variables),
      m_node_scoring_function(t_src.m_node_scoring_function ? t_src.m_node_scoring_function->clone() : nullptr),
      m_phases(t_src.m_phases)
{}

#endif //IDOL_STRONGBRANCHING_H
