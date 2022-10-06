//
// Created by henri on 20/09/22.
//

#ifndef OPTIMIZE_SOLVE_WITH_BRANCH_AND_PRICE_H
#define OPTIMIZE_SOLVE_WITH_BRANCH_AND_PRICE_H

#include "Instance.h"
#include "algorithms.h"

#include "../../tests/instances/generalized-assignment-problem/AbstractInstanceGAP.h"

void solve_with_branch_and_price(const AbstractInstanceGAP& t_instance) {

    const unsigned int n_knapsacks = t_instance.n_knapsacks();
    const unsigned int n_items = t_instance.n_items();
    const auto p = t_instance.p();
    const auto w = t_instance.w();
    const auto c = t_instance.c();

    std::vector<Var> branching_candidates;

    // SP
    std::vector<Model> subproblems;
    subproblems.reserve(n_knapsacks);
    std::vector<std::vector<Var>> x(n_knapsacks);

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        subproblems.emplace_back();

        x[i].reserve(n_items);

        for (unsigned int j = 0 ; j < n_items ; ++j) {
            x[i].emplace_back(subproblems.back().add_variable(0., 1., Binary, p[i][j], "x(" + std::to_string(i) + "," + std::to_string(j) + ")") );

            branching_candidates.emplace_back(x[i].back());
        }

        Expr expr;
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            expr += w[i][j] * x[i][j];
        }
        subproblems.back().add_constraint(expr <= c[i]);

    }

    // RMP
    Model rmp;

    std::vector<std::vector<Param>> param_x(n_knapsacks);

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        param_x[i].reserve(n_items);
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            param_x[i].emplace_back( rmp.add_parameter(x[i][j]) );
        }
    };

    for (unsigned int j = 0 ; j < n_items ; ++j) {
        Constant expr = 1;
        for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
            expr += -1. * param_x[i][j];
        }
        rmp.add_constraint(Expr() == expr, "assign(" + std::to_string(j) + ")");
    }

    // Algorithm
    auto solver = branch_and_price(rmp, subproblems.begin(), subproblems.end(), branching_candidates);

    solver.solve();

    std::cout << solver.status() << std::endl;
    std::cout << solver.objective_value() << std::endl;
    std::cout << "N. nodes: " << solver.n_created_nodes() << std::endl;

}

#endif //OPTIMIZE_SOLVE_WITH_BRANCH_AND_PRICE_H
