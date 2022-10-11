//
// Created by henri on 20/09/22.
//

#ifndef OPTIMIZE_SOLVE_WITH_EXTERNAL_SOLVER_H
#define OPTIMIZE_SOLVE_WITH_EXTERNAL_SOLVER_H

#include "Instance.h"

#include "../../tests/instances/generalized-assignment-problem/AbstractInstanceGAP.h"
#include "solvers.h"

void solve_with_external_solver(const AbstractInstanceGAP& t_instance) {

    const unsigned int n_knapsacks = t_instance.n_knapsacks();
    const unsigned int n_items = t_instance.n_items();
    const auto p = t_instance.p();
    const auto w = t_instance.w();
    const auto c = t_instance.c();

    Model model;

    std::vector<std::vector<Var>> x(n_knapsacks);
    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        x[i].reserve(n_items);
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            x[i].emplace_back(model.add_variable(0., 1., Binary, p[i][j], "x(" + std::to_string(i) + "," + std::to_string(j) + ")") );
        }
    };


    std::vector<Ctr> knapsack_constraints;
    knapsack_constraints.reserve(n_knapsacks);

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        Expr expr;
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            expr += w[i][j] * x[i][j];
        }
        knapsack_constraints.emplace_back(model.add_constraint(expr <= c[i]) );
    }



    std::vector<Ctr> assignment_constraints;
    assignment_constraints.reserve(n_items);

    for (unsigned int j = 0 ; j < n_items ; ++j) {
        Expr expr;
        for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
            expr += x[i][j];
        }
        assignment_constraints.emplace_back(model.add_constraint(expr == 1) );
    }

    std::tuple_element_t<0, available_solvers> solver(model);
    solver.solve();

    std::cout << solver.primal_solution().status() << std::endl;
    std::cout << solver.primal_solution().objective_value() << std::endl;

}

#endif //OPTIMIZE_SOLVE_WITH_EXTERNAL_SOLVER_H
