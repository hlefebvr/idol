//
// Created by henri on 20/09/22.
//

#ifndef OPTIMIZE_SOLVE_WITH_EXTERNAL_SOLVER_H
#define OPTIMIZE_SOLVE_WITH_EXTERNAL_SOLVER_H

#include "problems/gap/GAP_Instance.h"
#include "solvers.h"

void solve_with_external_solver(const Problems::GAP::Instance& t_instance) {

    const unsigned int n_knapsacks = t_instance.n_knapsacks();
    const unsigned int n_items = t_instance.n_items();

    Model model;

    std::vector<std::vector<Var>> x(n_knapsacks);
    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        x[i].reserve(n_items);
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            x[i].emplace_back(model.add_variable(0., 1., Binary, t_instance.p(i, j), "x(" + std::to_string(i) + "," + std::to_string(j) + ")") );
        }
    };


    std::vector<Ctr> knapsack_constraints;
    knapsack_constraints.reserve(n_knapsacks);

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        LinExpr expr;
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            expr += t_instance.w(i, j) * x[i][j];
        }
        knapsack_constraints.emplace_back(model.add_constraint(expr <= t_instance.t(i)) );
    }



    std::vector<Ctr> assignment_constraints;
    assignment_constraints.reserve(n_items);

    for (unsigned int j = 0 ; j < n_items ; ++j) {
        LinExpr expr;
        for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
            expr += x[i][j];
        }
        assignment_constraints.emplace_back(model.add_constraint(expr == 1) );
    }

    std::tuple_element_t<0, milp_solvers> solver(model);
    solver.solve();

    const auto primal_solution = solver.primal_solution();

    std::cout << primal_solution.status() << std::endl;
    std::cout << primal_solution.objective_value() << std::endl;
    std::cout << "Total time: " << solver.time().count() << " s" << std::endl;

}

#endif //OPTIMIZE_SOLVE_WITH_EXTERNAL_SOLVER_H
