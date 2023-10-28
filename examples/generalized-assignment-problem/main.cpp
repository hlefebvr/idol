//
// Created by henri on 13/03/23.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/optimizers/column-generation/Optimizers_ColumnGeneration.h"
#include "idol/optimizers/column-generation/ColumnGeneration.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/optimizers/column-generation/IntegerMaster.h"
#include "idol/optimizers/callbacks/RENS.h"
#include "idol/optimizers/callbacks/LocalBranching.h"
#include "idol/optimizers/callbacks/SimpleRounding.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/wrappers/Mosek/Mosek.h"

int main(int t_argc, const char** t_argv) {

    using namespace idol;

    const auto instance = Problems::GAP::read_instance("/home/henri/Research/idol/tests/data/generalized-assignment-problem/GAP_instance0.txt");

    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

    // Create optimization environment
    Env env;

    // Create model
    Model model(env);

    // Create decomposition annotation
    Annotation<Ctr> decomposition(env, "decomposition", MasterId);

    // Create assignment variables (x_ij binaries)
    auto x = model.add_vars(Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");

    // Create knapsack constraints (i.e., capacity constraints)
    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        auto capacity = model.add_ctr(idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));

        capacity.set(decomposition, i); // Assign constraint to i-th subproblem
    }

    // Create assignment constraints
    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
    }

    // Set the objective function
    model.set_obj_expr(idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    // Set optimizer
    model.use(BranchAndBound()
                  .with_node_optimizer(
                          DantzigWolfeDecomposition(decomposition)
                                  .with_master_optimizer(Mosek::ContinuousRelaxation())
                                  .with_pricing_optimizer(Mosek())
                                  .with_log_level(Info, Yellow)
                                  .with_farkas_pricing(true)
                                  .with_artificial_variables_cost(1e+4)
                                  .with_branching_on_master(false)
                                  .with_dual_price_smoothing_stabilization(.3)
                                  .with_column_pool_clean_up(1e+8, .75)
                  )
                .with_subtree_depth(0)
                .with_branching_rule(MostInfeasible())
                .with_node_selection_rule(WorstBound())
                .with_log_level(Info, Blue)
                .with_log_frequency(1)
                .with_callback(Heuristics::IntegerMaster().with_optimizer(Mosek()))
                /*
                .with_callback(Heuristics::IntegerMaster().with_optimizer(GLPK()))
                      .with_callback(
                              Heuristics::LocalBranching()
                                      .with_optimizer(
                                              BranchAndBound()
                                                      .with_node_optimizer(GLPK::ContinuousRelaxation())
                                                      .with_branching_rule(MostInfeasible())
                                                      .with_node_selection_rule(WorstBound())
                                                      .with_callback(Heuristics::SimpleRounding())
                                                      .with_log_level(Info, Green)
                                      )
                      )
                      */
            );

    // Solve
    model.optimize();

    // Print solution
    std::cout << save_primal(model) << std::endl;

    return 0;
}
