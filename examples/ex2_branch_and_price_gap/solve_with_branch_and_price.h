//
// Created by henri on 20/09/22.
//

#ifndef OPTIMIZE_SOLVE_WITH_BRANCH_AND_PRICE_H
#define OPTIMIZE_SOLVE_WITH_BRANCH_AND_PRICE_H

#include "problems/GAP/GAP_Instance.h"
#include "modeling.h"
#include "algorithms.h"

void solve_with_branch_and_price(const Problems::GAP::Instance& t_instance) {

    const unsigned int n_knapsacks = t_instance.n_knapsacks();
    const unsigned int n_items = t_instance.n_items();

    std::vector<Var> branching_candidates;

    Model rmp;
    std::vector<Var> alpha;
    alpha.reserve(n_knapsacks);

    // SP
    std::vector<Model> subproblems;
    subproblems.reserve(n_knapsacks);
    std::vector<std::vector<Var>> x(n_knapsacks);

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        subproblems.emplace_back();

        Constant objective_cost;

        x[i].reserve(n_items);

        for (unsigned int j = 0 ; j < n_items ; ++j) {
            x[i].emplace_back(subproblems.back().add_var(0., 1., Continuous, 0.,
                                                         "x(" + std::to_string(i) + "," + std::to_string(j) + ")") );

            objective_cost += t_instance.p(i, j) * !x[i][j];

            branching_candidates.emplace_back(x[i].back());
        }

        LinExpr expr;
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            expr += t_instance.w(i, j) * x[i][j];
        }
        subproblems.back().add_ctr(expr <= t_instance.t(i));

        alpha.emplace_back(rmp.add_var(0., 1., Continuous, std::move(objective_cost), "alpha") );

    }

    for (unsigned int j = 0 ; j < n_items ; ++j) {
        LinExpr expr;
        for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
            expr += !x[i][j] * alpha[i];
        }
        rmp.add_ctr(expr == 1., "assign(" + std::to_string(j) + ")");
    }

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        rmp.add_ctr(alpha[i] == 1., "convexity(" + std::to_string(i) + ")");
    }

    // Algorithm
    auto solver = branch_and_price<Solvers::GLPK_Simplex, Solvers::GLPK_Simplex>(rmp, alpha, subproblems, branching_candidates);

    solver.solve();

    const auto primal_solution = solver.primal_solution();

    std::cout << primal_solution.status() << std::endl;
    std::cout << primal_solution.objective_value() << std::endl;
    std::cout << "Total time: " << solver.time().count() << " s" << std::endl;
}

#endif //OPTIMIZE_SOLVE_WITH_BRANCH_AND_PRICE_H
