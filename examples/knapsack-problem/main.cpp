//
// Created by henri on 06/04/23.
//
#include <iostream>
#include "problems/knapsack-problem/KP_Instance.h"
#include "modeling.h"
#include "optimizers/solvers/Gurobi.h"

int main(int t_argc, const char** t_argv) {

    auto instance = Problems::KP::read_instance("instance.txt");

    const auto n_items = instance.n_items();

    Env env;

    // Create model
    Model model(env);

    auto x = model.add_vars(Dim<1>(n_items), 0, 1, Binary, "x");

    model.add_ctr(idol_Sum(j, Range(n_items), instance.weight(j) * x[j]) <= instance.capacity());

    model.set_obj_expr(idol_Sum(j, Range(n_items), -instance.profit(j) * x[j]));

    // Set optimizer
    model.use(Gurobi());

    // Solve
    model.optimize();

    std::cout << "Objective value = " << model.get_best_obj() << std::endl;

    std::cout << save_primal(model) << std::endl;

    return 0;
}
