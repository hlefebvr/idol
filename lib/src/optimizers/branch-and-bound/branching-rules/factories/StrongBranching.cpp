//
// Created by henri on 18.10.23.
//
#include "idol/optimizers/branch-and-bound/branching-rules/factories/StrongBranching.h"

idol::StrongBranching &idol::StrongBranching::with_max_n_variables(unsigned int t_n_variables) {

    if (m_max_n_variables.has_value()) {
        throw Exception("Max. number of variables has already been configured.");
    }

    m_max_n_variables = t_n_variables;

    return *this;
}

idol::StrongBranching &
idol::StrongBranching::with_node_scoring_function(const idol::NodeScoreFunction &t_score_function) {

    if (m_node_scoring_function) {
        throw Exception("Node scoring function has already been configured.");
    }

    m_node_scoring_function.reset(t_score_function.clone());

    return *this;
}

idol::StrongBranching &
idol::StrongBranching::add_phase(const StrongBranchingPhaseType &t_phase,
                                 unsigned int t_max_n_variables,
                                 unsigned int t_max_depth) {

    m_phases.emplace_back(t_phase, t_max_n_variables, t_max_depth);

    return *this;
}
