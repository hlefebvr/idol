//
// Created by henri on 06/04/23.
//
#include <iostream>
#include "problems/knapsack-problem/KP_Instance.h"
#include "modeling.h"
#include "optimizers/solvers/KnapsackSolver.h"
#include "optimizers/solvers/Gurobi.h"

int main(int t_argc, const char** t_argv) {

    auto instance = Problems::KP::read_instance("instance.txt");

    const auto n_items = instance.n_items();

    Env env;

    Model model(env);

    auto x = Var::make_vector(env, Dim<1>(n_items), 0, 1, Binary, "x");

    Ctr knapsack_constraint(env, idol_Sum(j, Range(n_items), instance.weight(j) * x[j]) <= instance.capacity());

    model.add_vector<Var, 1>(x);
    model.add(knapsack_constraint);
    model.set_obj(idol_Sum(j, Range(n_items), -instance.profit(j) * x[j]));

    ///

    model.use(Gurobi());

    model.optimize();

    std::cout << "Gurobi: " << model.get_best_obj() << std::endl;

    std::cout << save(model, Attr::Solution::Primal) << std::endl;

    model.use(KnapsackSolver());

    model.optimize();

    std::cout << "KnapsackSolver: " << model.get_best_obj() << std::endl;

    std::cout << save(model, Attr::Solution::Primal) << std::endl;

    return 0;
}
