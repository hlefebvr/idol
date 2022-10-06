//
// Created by henri on 16/09/22.
//
#include <iostream>
#include "modeling.h"
#include "algorithms.h"

int main() {

    Log::set_level(Trace);

    Model model;

    const std::vector<std::pair<double, double>> items = { {2, 40}, {3.14, 50}, {1.98, 100}, {5, 95}, {3, 30} }; // (weight, value)
    const double capacity = 10.;

    std::vector<Var> x;
    x.reserve(items.size());

    Expr sum_weight;

    for (const auto& [weight, profit] : items) {
        auto var = model.add_variable(0., 1., Continuous, -profit);
        sum_weight += weight * var;
        x.emplace_back(var);
    }

    model.add_constraint(sum_weight <= capacity);

    auto solver = branch_and_bound(model, x);
    solver.solve();

    std::cout << "Solution status = " << solver.status() << std::endl;
    std::cout << "Objective value = " << solver.objective_value() << std::endl;

    return 0;
}

