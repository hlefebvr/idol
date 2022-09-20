//
// Created by henri on 20/09/22.
//

#ifndef OPTIMIZE_SOLVE_WITH_LPSOLVE_H
#define OPTIMIZE_SOLVE_WITH_LPSOLVE_H

#include "Instance.h"
#include "solvers/lpsolve/Lpsolve.h"

void solve_with_lpsolve(const Instance& t_instance) {

    Env env;

    Model model(env);

    auto x = t_instance.create_variables(model, Integer);
    auto knapsack_constraints = t_instance.create_knapsack_constraints(model, x);
    auto assignment_constraints = t_instance.create_assignment_constraints(model, x);

    Lpsolve solver(model);
    solver.solve();

    std::cout << solver.primal_solution().status() << std::endl;
    std::cout << solver.primal_solution().objective_value() << std::endl;

}

#endif //OPTIMIZE_SOLVE_WITH_LPSOLVE_H
