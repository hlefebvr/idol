#include <iostream>

#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/SubProblem.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"

using namespace idol;

int main() {
    Env env;
    auto [model, decomposition] = DantzigWolfe::read_from_striplib(
            env, "striplib-import.data.lp", "striplib-import.data.dec");

    const auto column_generation = DantzigWolfeDecomposition(decomposition)
            .with_master_optimizer(GLPK::ContinuousRelaxation())
            .with_default_sub_problem_spec(DantzigWolfe::SubProblem().add_optimizer(GLPK()));
    const auto branch_and_bound = BranchAndBound()
            .with_branching_rule(MostInfeasible())
            .with_node_selection_rule(BestBound());

    model.use(branch_and_bound + column_generation);
    model.optimize();

    if (model.get_status() != Optimal) {
        std::cerr << "The imported decomposition was not solved to optimality.\n";
        return 1;
    }
    std::cout << "Objective value: " << model.get_best_obj() << '\n';
    return 0;
}
