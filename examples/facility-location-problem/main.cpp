//
// Created by henri on 06/04/23.
//
#include <iostream>
#include "modeling.h"
#include "problems/facility-location-problem/FLP_Instance.h"
#include "optimizers/branch-and-bound/BranchAndBound.h"
#include "optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "optimizers/solvers/Gurobi.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"

int main(int t_argc, const char** t_argv) {

    Env env;

    using namespace Problems::FLP;

    // Read instance
    const auto instance = read_instance_1991_Cornuejols_et_al("instance.txt");
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    // Make model
    auto x = Var::make_vector(env, Dim<1>(n_facilities), 0., 1., Binary, "x");
    auto y = Var::make_vector(env, Dim<2>(n_facilities, n_customers), 0., 1., Continuous, "y");

    Model model(env);

    model.add_vector<Var, 1>(x);
    model.add_vector<Var, 2>(y);

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add(Ctr(env, idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i) * x[i]));
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add(Ctr(env, idol_Sum(i, Range(n_facilities), y[i][j]) == 1));
    }

    model.set_obj_expr(idol_Sum(i, Range(n_facilities), instance.fixed_cost(i) * x[i] + idol_Sum(j, Range(n_customers),
                                                                                                 instance.per_unit_transportation_cost(
                                                                                                         i, j) *
                                                                                                 instance.demand(j) *
                                                                                                 y[i][j])));

    // Set backend options
    model.use(
            BranchAndBound()
                .with_node_solver(Gurobi::ContinuousRelaxation())
                .with_branching_rule(MostInfeasible())
                .with_node_selection_rule(BestBound())
                .with_log_level(Trace, Blue)
    );

    model.optimize();

    std::cout << model.get_status() << std::endl;


    return 0;
}
