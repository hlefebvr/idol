//
// Created by henri on 20/09/22.
//

#ifndef OPTIMIZE_SOLVE_WITH_BRANCH_AND_BOUND_H
#define OPTIMIZE_SOLVE_WITH_BRANCH_AND_BOUND_H

#include "Instance.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"

void solve_with_branch_and_bound(const Instance& t_instance) {

    Env env;

    Model model(env);

    auto x = t_instance.create_variables(model, Continuous);
    auto knapsack_constraints = t_instance.create_knapsack_constraints(model, x);
    auto assignment_constraints = t_instance.create_assignment_constraints(model, x);

    std::vector<Var> branching_candidates;
    std::copy(model.variables().begin(), model.variables().end(), std::back_inserter(branching_candidates));

    BranchAndBound solver(model, branching_candidates);
    solver.solve();

    std::cout << solver.status() << std::endl;
    std::cout << solver.objective_value() << std::endl;
    std::cout << "N. nodes: " << solver.n_created_nodes() << std::endl;

}

#endif //OPTIMIZE_SOLVE_WITH_BRANCH_AND_BOUND_H
