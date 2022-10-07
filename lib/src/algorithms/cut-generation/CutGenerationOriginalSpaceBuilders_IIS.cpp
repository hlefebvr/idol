//
// Created by henri on 06/10/22.
//
#include "algorithms/cut-generation/CutGenerationOriginalSpaceBuilders_IIS.h"
#include "algorithms/cut-generation/CutGenerationSubproblem.h"
#include "algorithms/Algorithm.h"
#include "modeling/models/Model.h"

CutGenerationOriginalSpaceBuilders::IIS::IIS(const Model& t_rmp)
    : m_rmp(t_rmp) {

}

Solution::Primal CutGenerationOriginalSpaceBuilders::IIS::primal_solution(const CutGenerationSubproblem &t_subproblem,
                                                                       const Algorithm &t_rmp_solution_strategy) const {

    Expr<Var> objective;
    for (const auto& [var, coeff] : m_rmp.objective()) {
        objective += coeff * var;
    }

    auto& rmp = const_cast<Algorithm&>(t_rmp_solution_strategy);
    auto infeasible = rmp.add_constraint(objective <= rmp.primal_solution().objective_value() - 1e-2);
    rmp.compute_iis();

    const auto iis = rmp.iis();

    rmp.remove_constraint(infeasible);

    Solution::Primal result;

    double sum_duals = 0.;

    for (const auto& [ctr, primal_solution] : t_subproblem.currently_present_cuts()) {
        const double dual = iis.get(ctr);

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

