#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/callbacks/ReducedCostFixing.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    /*****************/
    /* Instance data */
    /*****************/

    const Vector<double> fixed_costs { 109.6, 169.5, 24.8, 150.3, 140.7, 68.8, 5.4, 123.2, 118.5, 27.6, };
    const Vector<double> capacities { 131.5, 103.5, 41.8, 156.5, 128.5, 66.1, 33.7, 114.0, 152.3, 125.6, };
    const Vector<double> demands { 36.2, 5.6, 10.0, 20.2, 17.5, 35.8, 14.9, 2.8, 36.7, 2.8, 12.1, 16.3, 11.4, 9.0, 32.2, 13.8, 0.6, 26.9, 36.5, 10.0, };
    const Vector<double, 2> transportation_costs {
            {0.7, 0.4, 0.6, 0.7, 0.3, 0.6, 0.6, 0.6, 0.3, 0.3, 0.4, 0.6, 0.8, 0.6, 0.4, 0.2, 0.4, 0.3, 0.5, 0.4, },
            {0.8, 0.1, 0.3, 0.8, 0.7, 0.5, 0.7, 0.8, 0.2, 0.5, 0.2, 0.8, 0.8, 0.5, 0.6, 0.2, 0.6, 0.1, 0.8, 0.7, },
            {1.0, 0.5, 0.7, 1.0, 0.4, 0.9, 0.9, 0.9, 0.2, 0.6, 0.6, 0.9, 1.1, 0.9, 0.7, 0.3, 0.7, 0.4, 0.7, 0.5, },
            {0.8, 0.9, 1.1, 1.0, 0.3, 1.1, 0.9, 0.6, 0.8, 0.5, 0.9, 0.7, 1.0, 1.1, 0.6, 0.7, 0.5, 0.8, 0.1, 0.2, },
            {0.0, 0.7, 0.7, 0.2, 0.8, 0.6, 0.2, 0.2, 0.9, 0.4, 0.6, 0.1, 0.2, 0.5, 0.2, 0.7, 0.3, 0.7, 0.6, 0.8, },
            {0.4, 0.4, 0.4, 0.3, 0.7, 0.3, 0.2, 0.5, 0.6, 0.3, 0.3, 0.4, 0.4, 0.3, 0.3, 0.4, 0.3, 0.3, 0.7, 0.7, },
            {1.0, 0.6, 0.9, 1.1, 0.3, 1.0, 1.0, 0.9, 0.3, 0.6, 0.7, 1.0, 1.1, 1.0, 0.7, 0.4, 0.7, 0.5, 0.7, 0.4, },
            {0.1, 0.6, 0.6, 0.2, 0.8, 0.5, 0.2, 0.3, 0.8, 0.3, 0.5, 0.1, 0.2, 0.4, 0.2, 0.6, 0.2, 0.6, 0.6, 0.8, },
            {0.8, 0.6, 0.9, 0.9, 0.1, 0.9, 0.8, 0.7, 0.5, 0.4, 0.7, 0.8, 0.9, 0.9, 0.5, 0.4, 0.5, 0.5, 0.4, 0.2, },
            {0.7, 0.3, 0.5, 0.7, 0.4, 0.6, 0.7, 0.7, 0.2, 0.4, 0.3, 0.7, 0.8, 0.6, 0.5, 0.0, 0.5, 0.2, 0.6, 0.5, },
    };
    const auto n_facilities = fixed_costs.size();
    const auto n_customers = demands.size();

    const bool use_reduced_cost_fixing = true; // Set to false to disable reduced cost fixing

    /****************/
    /* Create model */
    /****************/

    // Create optimization environment
    Env env;

    // Create model
    Model model(env);

    // Add variables
    auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "x");
    auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Binary, 0., "y");

    // Add constraints

    // Capacity constraints
    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add_ctr(idol_Sum(j, Range(n_customers), demands[j] * y[i][j]) <= capacities[i] * x[i]);
    }

    // Demand constraints
    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_facilities), y[i][j]) == 1);
    }

    // Set objective function
    model.set_obj_expr(idol_Sum(i, Range(n_facilities),
                                fixed_costs[i] * x[i]
                                + idol_Sum(j, Range(n_customers),
                                             transportation_costs[i][j] * demands[j] * y[i][j]
                                         )
                                 )
                         );

    /*************************************/
    /* Create branch-and-bound algorithm */
    /*************************************/

    // First, we create a virgin branch-and-bound algorithm
    auto branch_and_bound = BranchAndBound();

    branch_and_bound.with_logs(true);

    // Configure how to solve the nodes' problems
    branch_and_bound.with_node_optimizer(GLPK::ContinuousRelaxation());

    // Configure node selection rule
    branch_and_bound.with_node_selection_rule(BestBound());

    // Configure branching rule
    branch_and_bound.with_branching_rule(PseudoCost());

    // If required, we add a reduced cost fixing callback
    if (use_reduced_cost_fixing) {
        branch_and_bound.add_callback(ReducedCostFixing());
    }

    /*******************/
    /* Solve the model */
    /*******************/

    model.use(branch_and_bound);
    model.optimize();

    // General functions
    const auto status = model.get_status();
    const auto reason = model.get_reason();
    const auto best_bound = model.get_best_bound();
    const auto best_obj = model.get_best_obj();
    const auto time = model.optimizer().time().count();

    // Branch-and-bound specific
    const auto& bb = model.optimizer().as<Optimizers::BranchAndBound<DefaultNodeInfo>>();
    const auto root_node_best_bound = bb.root_node_best_bound();
    const auto root_node_best_obj = bb.root_node_best_obj();
    const auto n_solved_nodes = bb.n_solved_nodes();

    std::cout << "Status: " << status << '\n';
    std::cout << "Reason: " << reason << '\n';
    std::cout << "Time: " << time << '\n';
    std::cout << "Best Obj.: " << best_obj << '\n';
    std::cout << "Best Bound: " << best_bound << '\n';
    std::cout << "Absolute Gap: " << absolute_gap(best_bound, best_obj) << '\n';
    std::cout << "Relative Gap: " << relative_gap(best_bound, best_obj) << '\n';
    std::cout << "Root Node Best Obj.: " << root_node_best_obj << '\n';
    std::cout << "Root Node Best Bound: " << root_node_best_bound << '\n';
    std::cout << "Root Node Absolute Gap: " << absolute_gap(root_node_best_bound, root_node_best_obj) << '\n';
    std::cout << "Root Node Relative Gap: " << relative_gap(root_node_best_bound, root_node_best_obj) << '\n';
    std::cout << "N. of Nodes: " << n_solved_nodes << '\n';
    std::cout << std::endl;

    return 0;
}
