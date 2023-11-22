#include "idol/modeling.h"
#include "idol/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"
#include "idol/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/optimizers/callbacks/heuristics/IntegerMaster.h"
#include "idol/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/optimizers/dantzig-wolfe/stabilization/Neame.h"
#include "idol/optimizers/dantzig-wolfe/stabilization/Wentges.h"
#include "idol/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h"
#include "idol/problems/facility-location-problem/FLP_Instance.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestEstimate.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/BracnhingWithPriority.h"
#include "idol/optimizers/wrappers/Mosek/Mosek.h"
#include "idol/problems/knapsack-problem/KP_Instance.h"
#include "idol/optimizers/callbacks/cutting-planes/KnapsackCover.h"

using namespace idol;

int main(int t_argc, char** t_argv) {

    const auto instance = Problems::KP::read_instance("/home/henri/Research/idol/examples/knapsack.data.txt");

    const auto n_items = instance.n_items();

    Env env;

    // Create model
    Model model(env);

    auto x = model.add_vars(Dim<1>(n_items), 0, 1, Binary, "x");

    auto c = model.add_ctr(idol_Sum(j, Range(n_items), instance.weight(j) * x[j]) <= instance.capacity());

    model.set_obj_expr(idol_Sum(j, Range(n_items), -instance.profit(j) * x[j]));

    std::cout << model << std::endl;

    model.scale_to_integers(Tolerance::Digits);

    std::cout << model << std::endl;

    auto row = model.get_ctr_row(c);

    std::cout << row << std::endl;

    std::cout << "scaling factor: " << row.scale_to_integers(Tolerance::Digits) << std::endl;

    std::cout << row << std::endl;

    std::cout << std::setprecision(8) << multiply_with_precision(1.52, 2.001, 2) << std::endl;

    return 0;

    // Set optimizer
    model.use(
                BranchAndBound()
                    .with_node_optimizer(Gurobi::ContinuousRelaxation())
                    .add_callback(Cuts::KnapsackCover())
                    .with_branching_rule(MostInfeasible())
                    .with_node_selection_rule(BestBound())
                    .with_log_level(Info, Blue)
            );

    // Solve
    model.optimize();

    std::cout << "Objective value = " << model.get_best_obj() << std::endl;

    std::cout << save_primal(model) << std::endl;

    return 0;
}
