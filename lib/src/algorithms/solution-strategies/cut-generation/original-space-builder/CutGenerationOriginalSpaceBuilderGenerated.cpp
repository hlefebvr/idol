//
// Created by henri on 06/10/22.
//
#include "algorithms/solution-strategies/cut-generation/original-space-builder/CutGenerationOriginalSpaceBuilderGenerated.h"
#include "algorithms/solution-strategies/cut-generation/subproblems/CutGenerationSubproblem.h"

Solution::Primal
CutGenerationOriginalSpaceBuilderGenerated::primal_solution(const CutGenerationSubproblem &t_subproblem, const Algorithm &t_rmp_duals) const {

    Solution::Primal result;

    for (const auto& [ctr, primal_solution] : t_subproblem.currently_present_cuts()) {
        result += primal_solution;
    }

    if (const unsigned int n_generated = t_subproblem.currently_present_cuts().size() ; n_generated > 1) {
        result *= 1. / n_generated;
    }

    return result;
}

