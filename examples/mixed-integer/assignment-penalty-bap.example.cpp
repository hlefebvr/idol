//
// Created by henri on 13/03/23.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/IntegerMaster.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/Neame.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/logs/Info.h"
#include "idol/mixed-integer/optimizers/padm/PenaltyMethod.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    const auto instance = Problems::GAP::read_instance("assignment-penalty-bap.data.txt");

    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

    // Create optimization environment
    Env env;

    // Create model
    Model model(env);

    // Create decomposition annotation
    Annotation decomposition(env, "decomposition", MasterId);

    // Create penalized constraints annotation
    Annotation<Ctr, bool> penalized_constraints(env, "penalized_constraints", false);

    // Create assignment variables (x_ij binaries)
    auto x = model.add_vars(Dim<2>(n_agents, n_jobs), 0., 1., Binary, 0., "x");

    // Create knapsack constraints (i.e., capacity constraints)
    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        auto capacity = model.add_ctr(idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));

        capacity.set(decomposition, i); // Assign constraint to i-th subproblem
    }

    // Create assignment constraints
    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        auto assignment = model.add_ctr(idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));

        assignment.set(penalized_constraints, true); // Penalize this constraint
    }

    // Set the objective function
    model.set_obj_expr(idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    const auto column_generation = DantzigWolfeDecomposition(decomposition)
            .with_master_optimizer(GLPK::ContinuousRelaxation().with_logs(false))
            .with_default_sub_problem_spec(
                    DantzigWolfe::SubProblem()
                            .add_optimizer(GLPK().with_logs(false))
                            .with_column_pool_clean_up(1500, .75)
            )
            .with_logger(Logs::DantzigWolfe::Info().with_frequency_in_seconds(.00000001))
            .with_dual_price_smoothing_stabilization(DantzigWolfe::Neame(.3))
            .with_infeasibility_strategy(DantzigWolfe::FarkasPricing())
            .with_hard_branching(true)
            .with_logs(true);


    const auto penalty_method = PenaltyMethod(penalized_constraints)
            .with_penalty_update(PenaltyUpdates::Multiplicative(2))
            .with_rescaling(true, 1e3)
            .with_feasible_solution_status(Optimal);

    const auto branch_and_bound = BranchAndBound()
            .with_subtree_depth(0)
            .with_branching_rule(MostInfeasible())
            .with_node_selection_rule(WorstBound())
            //.add_callback(Heuristics::IntegerMaster().with_osi_interface(HiGHS().with_logs(false)))
            .with_logs(true);

    // Set optimizer
    model.use(branch_and_bound + (penalty_method + column_generation));

    // Solve
    model.optimize();

    // Print solution
    std::cout << save_primal(model) << std::endl;

    return 0;
}
