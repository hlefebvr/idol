//
// Created by henri on 06/04/23.
//
#include <iostream>
#include <gurobi_c++.h>
#include "idol/problems/knapsack-problem/KP_Instance.h"
#include "idol/modeling.h"
#include "idol/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/optimizers/branch-and-bound/cutting-planes/CoverCuts.h"
#include "idol/optimizers/callbacks/SimpleRounding.h"
#include "idol/optimizers/callbacks/RENS.h"
#include "idol/optimizers/callbacks/LocalBranching.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/VariableBranching.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/Diver.h"
#include "idol/optimizers/branch-and-bound/watchers/ExportBranchAndBoundTreeToCSV.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/DepthFirst.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BreadthFirst.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestEstimate.h"

int main(int t_argc, const char** t_argv) {

    using namespace idol;

    const auto instance = Problems::KP::read_instance("instance.txt");
    //const auto instance = Problems::KP::read_instance("/home/henri/Research/idol/examples/knapsack-problem/instance50.txt");

    const auto n_items = instance.n_items();

    Env env;

    // Create model
    Model model(env);

    auto x = model.add_vars(Dim<1>(n_items), 0, 1, Binary, "x");

    model.add_ctr(idol_Sum(j, Range(n_items), instance.weight(j) * x[j]) <= instance.capacity());

    model.set_obj_expr(idol_Sum(j, Range(n_items), -instance.profit(j) * x[j]));

    // Set optimizer
    model.use(
            BranchAndBound()
                    .with_node_optimizer(HiGHS::ContinuousRelaxation())
                    .with_branching_rule(PseudoCost())
                    .with_callback(Utils::ExportBranchAndBoundTreeToCSV("tree.csv"))
                    //.with_callback(Heuristics::SimpleRounding())
                    .with_node_selection_rule(BestEstimate())
                    .with_log_level(Info, Blue)
                    .with_log_frequency(1)
    );
    // model.use(HiGHS());
    // Solve
    model.optimize();

    std::cout << "Objective value = " << model.get_best_obj() << std::endl;

    std::cout << save_primal(model) << std::endl;

    return 0;
}
