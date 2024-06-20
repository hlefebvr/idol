//
// Created by henri on 13/03/23.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/mixed-integer-optimization/callbacks/heuristics/IntegerMaster.h"
#include "idol/optimizers/mixed-integer-optimization/callbacks/heuristics/SimpleRounding.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/HiGHS/HiGHS.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/MinKnap/MinKnap.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/stabilization/Neame.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/logs/Info.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    const auto instance = Problems::GAP::read_instance("assignment.data.txt");

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
                                      .with_master_optimizer(HiGHS::ContinuousRelaxation().with_logs(false))
                                      .with_default_sub_problem_spec(
                                              DantzigWolfe::SubProblem()
                                                                .add_optimizer(HiGHS().with_logs(false))
                                                                .with_column_pool_clean_up(1500, .75)
                                      )
                                      .with_logger(Logs::DantzigWolfe::Info().with_frequency_in_seconds(.00000001))
                                      .with_dual_price_smoothing_stabilization(DantzigWolfe::Neame(.3))
                                      .with_infeasibility_strategy(DantzigWolfe::FarkasPricing())
                                      .with_hard_branching(true)
                                      .with_logs(true)
                      )
                      .with_subtree_depth(0)
                      .with_branching_rule(MostInfeasible())
                      .with_node_selection_rule(WorstBound())
                      .add_callback(Heuristics::IntegerMaster().with_optimizer(HiGHS().with_logs(false)))
                      .with_logs(true)
            );

    // Solve
    model.optimize();

    // Print solution
    std::cout << save_primal(model) << std::endl;

    return 0;
}
