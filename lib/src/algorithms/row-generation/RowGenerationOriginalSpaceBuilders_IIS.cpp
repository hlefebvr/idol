//
// Created by henri on 06/10/22.
//
#include "../../../include/algorithms/row-generation/RowGenerationOriginalSpaceBuilders_IIS.h"
#include "../../../include/algorithms/row-generation/RowGenerationSP.h"
#include "../../../include/algorithms/Algorithm.h"
#include "../../../include/modeling/models/Model.h"
#include "../../../include/modeling/expressions/operators.h"

RowGenerationOriginalSpaceBuilders::IIS::IIS(const Model& t_rmp)
    : m_rmp(t_rmp) {

}

Solution::Primal RowGenerationOriginalSpaceBuilders::IIS::primal_solution(const RowGenerationSP &t_subproblem,
                                                                          const Algorithm &t_rmp_solution_strategy) const {

    LinExpr<Var> objective;
    for (const auto& [var, coeff] : m_rmp.objective()) {
        objective += coeff * var;
    }

    auto& rmp = const_cast<Algorithm&>(t_rmp_solution_strategy);
    auto infeasible = rmp.add_row(objective <= rmp.primal_solution().objective_value() - 1e-2);
    rmp.compute_iis();

    const auto iis = rmp.iis();

    rmp.remove(infeasible);

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
