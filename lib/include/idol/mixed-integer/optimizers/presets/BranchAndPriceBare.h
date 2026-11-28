#ifndef IDOL_PRESETS_BRANCH_AND_PRICE_BARE_H
#define IDOL_PRESETS_BRANCH_AND_PRICE_BARE_H

#include <utility>

#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/NodeWithCGInfo.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"

namespace idol::Presets {

/**
 * Branch-and-price without stabilisation.
 *
 * Uses MostInfeasible branching, BestBound node selection, and pure
 * Dantzig-Wolfe column generation (no dual-price smoothing).
 * Identical to BranchAndPrice() except that no stabilisation is applied.
 */
inline BranchAndBound<NodeWithCGInfo> BranchAndPriceBare(
        Annotation<unsigned int> t_decomposition,
        const OptimizerFactory& t_master_optimizer,
        const OptimizerFactory& t_pricing_optimizer) {
    auto column_generation = DantzigWolfeDecomposition(std::move(t_decomposition))
            .with_master_optimizer(t_master_optimizer)
            .with_default_sub_problem_spec(
                    DantzigWolfe::SubProblem().add_optimizer(t_pricing_optimizer));

    const auto branch_and_bound = BranchAndBound<NodeWithCGInfo>()
            .with_branching_rule(MostInfeasible())
            .with_node_selection_rule(BestBound());

    return branch_and_bound + column_generation;
}

}

#endif // IDOL_PRESETS_BRANCH_AND_PRICE_BARE_H
