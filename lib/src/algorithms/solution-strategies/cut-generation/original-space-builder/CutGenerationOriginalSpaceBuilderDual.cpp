//
// Created by henri on 06/10/22.
//
#include "algorithms/solution-strategies/cut-generation/original-space-builder/CutGenerationOriginalSpaceBuilderDual.h"
#include "algorithms/solution-strategies/cut-generation/subproblems/CutGenerationSubproblem.h"

Solution::Primal CutGenerationOriginalSpaceBuilderDual::primal_solution(const CutGenerationSubproblem &t_subproblem,
                                                                        const Algorithm &t_rmp_solution_strategy) const {

    const Solution::Dual t_rmp_duals = t_rmp_solution_strategy.dual_solution();

    Solution::Primal result;

    double sum_duals = 0.;

    for (const auto& [ctr, primal_solution] : t_subproblem.currently_present_cuts()) {
        const double dual = t_rmp_duals.get(ctr);

        if (!equals(dual, 0., ToleranceForSparsity)) {
            sum_duals += dual;
            result += dual * primal_solution;
        }

    }

    if (!equals(sum_duals, 1., ToleranceForSparsity)) {
        result *= 1. / sum_duals;
    }

    return result;
}

