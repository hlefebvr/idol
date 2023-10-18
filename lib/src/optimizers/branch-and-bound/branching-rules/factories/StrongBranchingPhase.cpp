//
// Created by henri on 18.10.23.
//
#include "optimizers/branch-and-bound/branching-rules/impls/strong-branching/StrongBranchingPhase.h"

idol::StrongBranchingPhase::StrongBranchingPhase(const StrongBranchingPhaseType &t_phase_type,
                                    unsigned int t_max_n_variables,
                                    unsigned int t_max_depth)
        : m_phase_type(t_phase_type.clone()),
          m_max_n_variables(t_max_n_variables),
          m_max_depth(t_max_depth) {

}

idol::StrongBranchingPhase::StrongBranchingPhase(const StrongBranchingPhase &t_src)
        : m_max_depth(t_src.m_max_depth),
          m_max_n_variables(t_src.m_max_n_variables),
          m_phase_type(t_src.m_phase_type->clone())
{}
