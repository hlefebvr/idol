//
// Created by henri on 06/04/23.
//
#include <iostream>
#include <gurobi_c++.h>
#include "problems/knapsack-problem/KP_Instance.h"
#include "modeling.h"
#include "optimizers/solvers/gurobi/Gurobi.h"
#include "optimizers/branch-and-bound/BranchAndBound.h"
#include "optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "optimizers/branch-and-bound/cutting-planes/CoverCuts.h"

int main(int t_argc, const char** t_argv) {

    using namespace idol;

    const auto instance = Problems::KP::read_instance("instance.txt");
    //const auto instance = Problems::KP::read_instance("/home/henri/CLionProjects/optimize/examples/knapsack-problem/instance50.txt");

    const auto n_items = instance.n_items();

    Env env;

    // Create model
    Model model(env);

    auto x = model.add_vars(Dim<1>(n_items), 0, 1, Binary, "x");

    model.add_ctr(idol_Sum(j, Range(n_items), instance.weight(j) * x[j]) <= instance.capacity());

    model.set_obj_expr(idol_Sum(j, Range(n_items), -instance.profit(j) * x[j]));

    // Set optimizer
    //model.use(Gurobi());
    model.use(
            BranchAndBound()
                    .with_node_optimizer(Gurobi::ContinuousRelaxation())
                    .with_branching_rule(MostInfeasible())
                    .with_node_selection_rule(BestBound())
                    //.with_cutting_planes(CoverCuts().with_optimizer(Gurobi()))
                    .with_log_level(Info, Blue)
    );
    // Solve
    model.optimize();

    std::cout << "Objective value = " << model.get_best_obj() << std::endl;

    std::cout << save_primal(model) << std::endl;

    return 0;
}
