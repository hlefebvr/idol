//
// Created by henri on 20/09/22.
//

#ifndef OPTIMIZE_SOLVE_WITH_BRANCH_AND_PRICE_H
#define OPTIMIZE_SOLVE_WITH_BRANCH_AND_PRICE_H

#include "Instance.h"
#include "algorithms/branch-and-cut-and-price/DantzigWolfeGenerator.h"
#include "algorithms/branch-and-bound/MostInfeasible.h"
#include "algorithms/branch-and-bound/NodeByBoundStrategy.h"
#include "algorithms/branch-and-cut-and-price/DecompositionStrategy.h"
#include "algorithms/branch-and-cut-and-price/ColumnGenerationStrategy.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "solvers/gurobi/Gurobi.h"
#include "solvers/lpsolve/Lpsolve.h"

void solve_with_branch_and_price(const Instance& t_instance) {

    const unsigned int n_knapsacks = t_instance.n_knapsacks;
    const unsigned int n_items = t_instance.n_items;
    const auto& p = t_instance.p;
    const auto& w = t_instance.w;
    const auto& c = t_instance.c;

    Env env;

    // RMP
    Model rmp(env);

    std::vector<std::vector<Var>> x(n_knapsacks);
    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        x[i].reserve(n_items);
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            x[i].emplace_back(rmp.add_virtual_variable(0., 1., Continuous, 0., "x(" + std::to_string(i) + "," + std::to_string(j) + ")") );
        }
    };

    t_instance.create_assignment_constraints(rmp, x);

    std::vector<Var> branching_candidates;
    std::copy(rmp.variables().begin(), rmp.variables().end(), std::back_inserter(branching_candidates));

    // Alg
    BranchAndBound solver;
    solver.set_node_strategy<NodeByBoundStrategy>();
    solver.set_branching_strategy<MostInfeasible>(branching_candidates);
    auto& generation_strategy = solver.set_solution_strategy<DecompositionStrategy<Gurobi>>(rmp);
    auto& column_generation = generation_strategy.add_generation_strategy<ColumnGenerationStrategy>();

    // SP

    std::vector<Model> subproblems;
    subproblems.reserve(n_knapsacks);

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        subproblems.emplace_back(env);

        std::vector<Var> x_bar;
        x_bar.reserve(n_items);

        for (unsigned int j = 0 ; j < n_items ; ++j) {
            x_bar.emplace_back(subproblems.back().add_variable(0., 1., Integer, -p[i][j], "x_bar(" + std::to_string(i) + "," + std::to_string(j) + ")") );
        }

        Expr expr;
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            expr += w[i][j] * x_bar[j];
        }
        subproblems.back().add_constraint(expr <= c[i]);

        auto convexity = rmp.add_constraint(Equal, 1);

        DantzigWolfeGenerator generator(rmp, subproblems.back(), convexity);

        for (unsigned int j = 0 ; j < n_items ; ++j) {
            generator.set(x[i][j], x_bar[j]);
        }

        column_generation.add_subproblem<ExternalSolverStrategy<Gurobi>>(generator, subproblems.back());

    }

    solver.solve();

    std::cout << solver.primal_solution() << std::endl;
    std::cout << "N. nodes: " << solver.n_created_nodes() << std::endl;

}

#endif //OPTIMIZE_SOLVE_WITH_BRANCH_AND_PRICE_H
