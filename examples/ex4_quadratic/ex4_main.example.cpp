//
// Created by henri on 29/11/22.
//
#include <iostream>
#include "modeling.h"
#include "algorithms.h"
#include "problems/KP/KP_Instance.h"
#include "algorithms/callbacks/Callbacks_RoundingHeuristic.h"

// Taken from https://optimization.mccormick.northwestern.edu/index.php/Quadratic_programming
int main() {

    Model model;
    auto x = model.add_vars(Dim<1>(2), 0, Inf, Continuous, 0);
    auto c = model.add_ctr(2 * x[0] + 3 * x[1] >= 4);

    model.set(Attr::Obj::Expr, 3 * x[0] * x[0] + x[1] * x[1] + 2 * x[0] * x[1] + x[0] + 6 * x[1] + 2);

    Solvers::Gurobi solver(model);
    solver.solve();

    std::cout << solver.primal_solution().objective_value() << std::endl;

    return 0;
}

