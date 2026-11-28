#ifndef IDOL_PRESETS_BRANCH_AND_PRICE_H
#define IDOL_PRESETS_BRANCH_AND_PRICE_H

#include <utility>

#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/NodeWithCGInfo.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/Wentges.h"
#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/CglCutCallback.h"

namespace idol::Presets {

/**
 * Recommended branch-and-price configuration.
 *
 * Uses:
 * - MostInfeasible branching
 * - BestBound node selection
 * - Dantzig-Wolfe column generation with Wentges dual-price smoothing (rho = 0.4)
 * - Cgl cutting planes at each node when idol is compiled with USE_CGL=ON
 *   (Cover, Flow Cover, Zero Half, MIR, Residual Capacity);
 *   gracefully omitted otherwise.
 *
 * For a configuration without stabilisation, see BranchAndPriceBare.
 */
inline BranchAndBound<NodeWithCGInfo> BranchAndPrice(
        Annotation<unsigned int> t_decomposition,
        const OptimizerFactory& t_master_optimizer,
        const OptimizerFactory& t_pricing_optimizer) {

    auto column_generation = DantzigWolfeDecomposition(std::move(t_decomposition))
            .with_master_optimizer(t_master_optimizer)
            .with_default_sub_problem_spec(
                    DantzigWolfe::SubProblem().add_optimizer(t_pricing_optimizer))
            .with_dual_price_smoothing_stabilization(DantzigWolfe::Wentges(.4))
            .with_max_root_cg_iterations(10'000);

    auto branch_and_bound = BranchAndBound<NodeWithCGInfo>()
            .with_branching_rule(MostInfeasible())
            .with_node_selection_rule(BestBound());

#ifdef IDOL_USE_CGL
    branch_and_bound.add_callback(CglCutCallback<NodeWithCGInfo>());
#endif

    return branch_and_bound + column_generation;
}

}

#endif // IDOL_PRESETS_BRANCH_AND_PRICE_H
