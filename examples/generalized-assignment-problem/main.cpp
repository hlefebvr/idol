//
// Created by henri on 13/03/23.
//
#include <iostream>
#include "modeling.h"
#include "problems/generalized-assignment-problem/GAP_Instance.h"
#include "optimizers/column-generation/Optimizers_ColumnGeneration.h"
#include "optimizers/column-generation/ColumnGeneration.h"
#include "optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "optimizers/branch-and-bound/BranchAndBound.h"
#include "optimizers/solvers/GLPK.h"
#include "optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"

int main(int t_argc, const char** t_argv) {

    const auto instance = Problems::GAP::read_instance("instance.txt");

    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

    // Create optimization environment
    Env env;

    // Create model
    Model model(env);

    // Create decomposition annotation
    Annotation<Ctr> decomposition(env, "decomposition", MasterId);

    // Create assignment variables (x_ij binaries)
    auto x = Var::array(env, Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");

    // Add variables to the model
    model.add_array<Var, 2>(x);

    // Create knapsack constraints (i.e., capacity constraints)
    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity(env, idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
        model.add(capacity);

        capacity.set(decomposition, i); // Assign constraint to i-th subproblem
    }

    // Create assignment constraints
    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        Ctr assignment(env, idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
        model.add(assignment);
    }

    // Set the objective function
    model.set(Attr::Obj::Expr, idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    // Set optimizer
    model.use(BranchAndBound()

                .with_node_solver(
                    DantzigWolfeDecomposition(decomposition)
                        .with_master_solver(GLPK::ContinuousRelaxation())
                        .with_pricing_solver(GLPK())
                        .with_log_level(Info, Yellow)
                        .with_farkas_pricing(false)
                        .with_artificial_variables_cost(1e+4)
                        .with_branching_on_master(true)
                        .with_dual_price_smoothing_stabilization(.3)
                        .with_column_pool_clean_up(1e+8, .75)
                )
                .with_branching_rule(MostInfeasible())
                .with_node_selection_rule(WorstBound())
                .with_log_level(Info, Blue)
            );

    // TODO: add primal heuristic

    // Solve
    model.optimize();

    // Print solution
    std::cout << save_primal_values(model) << std::endl;

    return 0;
}
