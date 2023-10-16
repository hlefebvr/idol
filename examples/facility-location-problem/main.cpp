//
// Created by henri on 06/04/23.
//
#include <iostream>
#include "modeling.h"
#include "problems/facility-location-problem/FLP_Instance.h"
#include "optimizers/branch-and-bound/BranchAndBound.h"
#include "optimizers/solvers/gurobi/Gurobi.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "optimizers/branch-and-bound/scoring-functions/MostFractional.h"
#include "optimizers/branch-and-bound/branching-rules/factories/VariableBranching.h"

int main(int t_argc, const char** t_argv) {

    using namespace idol;

    Env env;

    // Read instance
    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("instance.txt");
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    // Make model

    Model model(env);

    auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, "x");
    auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Continuous, "y");

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add_ctr(idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i) * x[i]);
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_facilities), y[i][j]) == 1);
    }

    model.set_obj_expr(idol_Sum(i, Range(n_facilities),
                                instance.fixed_cost(i) * x[i]
                                + idol_Sum(j, Range(n_customers),
                                             instance.per_unit_transportation_cost(i, j) *
                                             instance.demand(j) *
                                             y[i][j]
                                         )
                                 )
                         );

    // Set backend options
    model.use(
            BranchAndBound()
                    .with_node_optimizer(Gurobi::ContinuousRelaxation())
                .with_branching_rule(VariableBranching(MostFractional()))
                .with_node_selection_rule(BestBound())
                .with_log_level(Trace, Blue)
    );

    model.optimize();

    std::cout << model.get_status() << std::endl;

    return 0;
}
