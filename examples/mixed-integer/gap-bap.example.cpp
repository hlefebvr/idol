#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/Neame.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/IntegerMaster.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    /********/
    /* Data */
    /********/

    const Vector<double, 2> costs = {
            { -27, -12, -12, -16, -24, -31, -41, -13, },
            { -14, -5, -37, -9, -36, -25, -1, -35, },
            { -34, -34, -20, -9, -19, -19, -3, -24, },
    };
    const Vector<double, 2> resource_consumption = {
            { 21, 13, 9, 5, 7, 15, 5, 24, },
            { 20, 8, 18, 25, 6, 6, 9, 6, },
            { 16, 16, 18, 24, 11, 11, 16, 18, },
    };
    const Vector<double> capacities = { 26, 25, 34, };

    const unsigned int n_machines = capacities.size();
    const unsigned int n_jobs = costs.front().size();

    Env env;

    /************************************************/
    /* Create the model for the natural formulation */
    /************************************************/

    // Create model
    Model model(env);

    // Add variables
    auto x = model.add_vars(Dim<2>(n_machines, n_jobs), 0., 1., Binary, 0., "x");

    // Add constraints

    // Add capacity constraints
    for (unsigned int i = 0 ; i < n_machines ; ++i) {
        model.add_ctr(idol_Sum(j, Range(n_jobs), resource_consumption[i][j] * x[i][j]) <= capacities[i], "capacity_" + std::to_string(i));
    }

    // Add assignment constraints
    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_machines), x[i][j]) == 1, "assignment_" + std::to_string(j));
    }

    // Set the objective function
    model.set_obj_expr(idol_Sum(i, Range(n_machines), idol_Sum(j, Range(n_jobs), costs[i][j] * x[i][j])));

    /*****************************************************************/
    /* Annotate the model to perform the Dantzig-Wolfe decomposition */
    /*****************************************************************/

    // Create decomposition annotation
    Annotation decomposition(env, "decomposition", MasterId); // By default, everything remains in the master problem

    // Annotate constraints to be moved to the subproblems
    for (const auto& ctr : model.ctrs()) {
        if (ctr.name().starts_with("capacity_")) {
            const unsigned int machine_id = std::stoi(ctr.name().substr(9));
            ctr.set(decomposition, machine_id); // Assign constraint to machine's subproblem
        }
    }

    /*****************************************/
    /* Build the column generation algorithm */
    /*****************************************/

    // Create the Dantzig-Wolfe decomposition algorithm
    auto column_generation = DantzigWolfeDecomposition(decomposition);

    // Use Gurobi to solve the master problem
    column_generation.with_master_optimizer(Gurobi::ContinuousRelaxation());

    // All subproblems will be solved by Gurobi
    const auto subproblem_specifications = DantzigWolfe::SubProblem()
            .add_optimizer(Gurobi())
            .with_column_pool_clean_up(1500, .75); // If the msater contains more tham 1500 columns, this will automatically remove the first 25% generated columns (if not used)
    column_generation.with_default_sub_problem_spec(subproblem_specifications);

    // Use dual price stabilization à la Neame with a smoothing factor of 0.3
    column_generation.with_dual_price_smoothing_stabilization(DantzigWolfe::Neame(.3));

    // Use the Farkas pricing strategy to generate columns when the master problem is infeasible
    column_generation.with_infeasibility_strategy(DantzigWolfe::FarkasPricing()); // An alternative would be DantzigWolfe::ArtificialCosts

    // Apply branching to the master problem
    column_generation.with_hard_branching(false);

    // Turn on logs
    column_generation.with_logs(true);

    /*****************************************/
    /* Create the branch-and-bound algorithm */
    /*****************************************/

    auto branch_and_bound = BranchAndBound();

    // Use the most infeasible branching rule
    branch_and_bound.with_branching_rule(MostInfeasible());

    // Select nodes according to the best bound rule
    branch_and_bound.with_node_selection_rule(BestBound());

    // Add a heuristic which solves the master problem with integrality constraints applied to the master's variables
    branch_and_bound.add_callback(Heuristics::IntegerMaster().with_optimizer(Gurobi()));

    // Turn on logs
    branch_and_bound.with_logs(true);

    /*****************************************/
    /* Create the branch-and-price algorithm */
    /*****************************************/

    const auto branch_and_price = branch_and_bound + column_generation;

    /************************************/
    /* Solve the model by decomposition */
    /************************************/

    model.use(branch_and_price);
    model.optimize();

    // Print solution
    std::cout << save_primal(model) << std::endl;

    return 0;
}
