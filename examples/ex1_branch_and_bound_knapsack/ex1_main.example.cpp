//
// Created by henri on 16/09/22.
//
#include <iostream>
#include "modeling.h"
#include "algorithms.h"
#include "problems/KP/KP_Instance.h"
#include "algorithms/callbacks/Callbacks_RoundingHeuristic.h"

int main() {

    Logs::set_level<BranchAndBound>(Info);

    Model model;

    using namespace Problems::KP;

    const auto instance = read_instance("demo.txt");

    const unsigned int n_items = instance.n_items();

    std::vector<Var> x;
    x.reserve(n_items);

    LinExpr sum_weight;

    for (unsigned int i = 0 ; i < n_items ; ++i) {
        auto var = model.add_var(0., 1., Continuous, -instance.p(i));
        sum_weight += instance.w(i) * var;
        x.emplace_back(var);
    }

    model.add_ctr(sum_weight <= instance.t());

    auto solver = branch_and_bound(model, x);
    solver.add_callback<Callbacks::RoundingHeuristic>(x);
    //solver.set(Param::Algorithm::MaxIterations, 4);
    solver.solve();

    std::cout << "Solution status = " << solver.status() << std::endl;
    std::cout << "Objective value = " << solver.objective_value() << std::endl;

    return 0;
}

