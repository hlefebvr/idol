//
// Created by henri on 17.12.25.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/NodeWithCGInfo.h"
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

    const Vector<double> weights = { 2, 3.14, 1.98, 5, 3, };
    const double capacity = 7.;
    const unsigned int n_items = weights.size();
    const unsigned int n_max_bins = std::ceil(idol_Sum(j, Range(n_items), weights[j]) / capacity);

    Env env;

    /************************************************/
    /* Create the model for the natural formulation */
    /************************************************/

    // Create model
    Model model(env);
    Annotation decomposition(env, "decomposition", MasterId);

    // Variables
    auto x = model.add_vars(Dim<2>(n_items, n_max_bins), 0., 1., Binary, 0., "x");
    auto y = model.add_vars(Dim<1>(n_max_bins), 0., 1., Binary, 1., "y");

    // Capacity constraints
    for (unsigned int k = 0 ; k < n_max_bins ; ++k) {
        const auto c = model.add_ctr(idol_Sum(j, Range(n_items), weights[j] * x[j][k]) <= capacity, "knapsack_" + std::to_string(k) );
        c.set(decomposition, k);
    }

    // Assignment constraints
    for (unsigned int j = 0 ; j < n_items ; ++j) {
        model.add_ctr(idol_Sum(k, Range(n_max_bins), x[j][k]) == 1, "assignment_" + std::to_string(j));
    }

    // Assignment constraints
    for (unsigned int j = 0 ; j < n_items ; ++j) {
        for (unsigned int k = 0 ; k < n_max_bins ; ++k) {
            model.add_ctr(x[j][k] <= y[k], "assignment_" + std::to_string(j) + "_" + std::to_string(k));
        }
    }

    /*****************************************/
    /* Build the column generation algorithm */
    /*****************************************/

    DantzigWolfe::SubProblem sub_problem;
    sub_problem.add_optimizer(Gurobi());
    sub_problem.with_column_pool_clean_up(1500, .75);
    sub_problem.with_max_column_per_pricing(10);

    const auto column_generation = DantzigWolfeDecomposition(decomposition)
            .with_master_optimizer(Gurobi::ContinuousRelaxation())
            .with_default_sub_problem_spec(sub_problem)
            .with_dual_price_smoothing_stabilization(DantzigWolfe::Neame(.4))
            .with_hard_branching(false)
            .with_infeasibility_strategy(DantzigWolfe::FarkasPricing())
            .with_max_parallel_sub_problems(1);

    /*****************************************/
    /* Create the branch-and-bound algorithm */
    /*****************************************/

    const auto branch_and_bound = BranchAndBound<NodeWithCGInfo>()
        .with_branching_rule(MostInfeasible())
        .with_node_selection_rule(BestBound())
        .add_callback(Heuristics::IntegerMaster<NodeWithCGInfo>().with_optimizer(Gurobi()))
        .with_logs(true)
    ;

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

    // Extract active columns in the incumbent
    std::cout << "*** Active Columns ***" << std::endl;
    const auto& active_columns = NodeWithCGInfo::get_active_columns(model);
    const unsigned int n_sub_problems = active_columns.size();
    for (unsigned int sub_problem_id = 0 ; sub_problem_id < n_sub_problems ; sub_problem_id++) {
        std::cout << "Sub Problem " << sub_problem_id << ":" << std::endl;
        for (const auto& [coefficient, column] : active_columns[sub_problem_id]) {
            std::cout << coefficient << "\n" << column << std::endl;
        }
    }

    return 0;
}
