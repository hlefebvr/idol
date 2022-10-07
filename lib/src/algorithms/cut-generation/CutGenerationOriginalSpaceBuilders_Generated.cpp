//
// Created by henri on 06/10/22.
//
#include "algorithms/cut-generation/CutGenerationOriginalSpaceBuilders_Generated.h"
#include "algorithms/cut-generation/CutGenerationSubproblem.h"

Solution::Primal
CutGenerationOriginalSpaceBuilders::Generated::primal_solution(const CutGenerationSubproblem &t_subproblem, const Algorithm &t_rmp_duals) const {

    Solution::Primal result;

    for (const auto& [ctr, primal_solution] : t_subproblem.currently_present_cuts()) {
        result += primal_solution;
    }

    if (const unsigned int n_generated = t_subproblem.currently_present_cuts().size() ; n_generated > 1) {
        result *= 1. / n_generated;
    }

    return result;
}

