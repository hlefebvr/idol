//
// Created by henri on 06/10/22.
//
#include "../../../include/algorithms/cut-generation/CutGenerationOriginalSpaceBuilders_Generated.h"
#include "../../../include/algorithms/cut-generation/CutGenerationSP.h"

Solution::Primal
CutGenerationOriginalSpaceBuilders::Generated::primal_solution(const CutGenerationSP &t_subproblem, const Algorithm &t_rmp_duals) const {

    Solution::Primal result;

    for (const auto& [ctr, primal_solution] : t_subproblem.currently_present_cuts()) {
        result += primal_solution;
    }

    if (const unsigned int n_generated = t_subproblem.currently_present_cuts().size() ; n_generated > 1) {
        result *= 1. / n_generated;
    }

    return result;
}

