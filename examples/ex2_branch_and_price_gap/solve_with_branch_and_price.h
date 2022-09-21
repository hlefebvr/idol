//
// Created by henri on 20/09/22.
//

#ifndef OPTIMIZE_SOLVE_WITH_BRANCH_AND_PRICE_H
#define OPTIMIZE_SOLVE_WITH_BRANCH_AND_PRICE_H

#include "Instance.h"
#include "algorithms/branch-and-cut-and-price/DantzigWolfe_RMP_Strategy.h"
#include "algorithms/branch-and-bound/MostInfeasible.h"
#include "algorithms/branch-and-bound/NodeByBoundStrategy.h"
#include "algorithms/branch-and-cut-and-price/DecompositionStrategy.h"
#include "algorithms/branch-and-cut-and-price/ColumnGenerationStrategy.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "solvers/gurobi/Gurobi.h"
#include "solvers/lpsolve/Lpsolve.h"
#include "algorithms/branch-and-bound/AbstractActiveNodeManagerWithTypeStrategy.h"
#include "algorithms/branch-and-bound/AbstractBranchingStrategy_2.h"
#include "algorithms/branch-and-bound/AbstractNodeUpdatorStrategy.h"
#include "algorithms/branch-and-bound/NodeStorageStrategy.h"

void solve_with_branch_and_price(const Instance& t_instance) {

    const unsigned int n_knapsacks = t_instance.n_knapsacks;
    const unsigned int n_items = t_instance.n_items;
    const auto& p = t_instance.p;
    const auto& w = t_instance.w;
    const auto& c = t_instance.c;

    Env env;

    std::vector<Var> branching_candidates;

    // SP
    std::vector<Model> subproblems;
    subproblems.reserve(n_knapsacks);
    std::vector<std::vector<Var>> x(n_knapsacks);

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        subproblems.emplace_back(env);

        x[i].reserve(n_items);

        for (unsigned int j = 0 ; j < n_items ; ++j) {
            x[i].emplace_back(subproblems.back().add_variable(0., 1., Binary, -p[i][j], "x(" + std::to_string(i) + "," + std::to_string(j) + ")") );

            branching_candidates.emplace_back(x[i].back());
        }

        Expr expr;
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            expr += w[i][j] * x[i][j];
        }
        subproblems.back().add_constraint(expr <= c[i]);

    }

    // RMP
    Model rmp(env);

    std::vector<std::vector<Param>> param_x(n_knapsacks);

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        param_x[i].reserve(n_items);
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            param_x[i].emplace_back( rmp.add_parameter(x[i][j]) );
        }
    };

    for (unsigned int j = 0 ; j < n_items ; ++j) {
        Coefficient expr = 1;
        for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
            expr += -1. * param_x[i][j];
        }
        rmp.add_constraint(Expr() == expr, "assign(" + std::to_string(j) + ")");
    }

    // Algorithm
    BranchAndBound solver;
    auto& node_strategy = solver.set_node_storage_strategy<NodeStorageStrategy<NodeByBound>>();
    node_strategy.set_active_node_manager_strategy<ActiveNodeManager_Heap>();
    node_strategy.set_node_updator_strategy<NodeUpdatorByBound>();
    node_strategy.set_branching_strategy<MostInfeasible_2>(std::move(branching_candidates));
    auto& generation_strategy = solver.set_solution_strategy<DecompositionStrategy>();
    auto& rmp_solver = generation_strategy.set_rmp_solution_strategy<ExternalSolverStrategy<Lpsolve>>(rmp);
    auto& column_generation = generation_strategy.add_generation_strategy<ColumnGenerationStrategy>();

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        auto& sp = column_generation.add_subproblem();
        sp.set_solution_strategy<ExternalSolverStrategy<Lpsolve>>(subproblems[i]);
        sp.set_generation_strategy<DantzigWolfe_RMP_Strategy>(rmp, subproblems[i]);
    }

    solver.solve();

    std::cout << solver.status() << std::endl;
    std::cout << solver.objective_value() << std::endl;
    std::cout << "N. nodes: " << solver.n_created_nodes() << std::endl;

}

#endif //OPTIMIZE_SOLVE_WITH_BRANCH_AND_PRICE_H
