#include <iostream>
#include "modeling.h"
#include "solvers/gurobi/Gurobi.h"
#include "solvers/lpsolve/Lpsolve.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "algorithms/logs/Log.h"
#include "algorithms/branch-and-cut-and-price/DecompositionStrategy.h"
#include "algorithms/branch-and-bound/MostInfeasible.h"
#include "algorithms/branch-and-bound/NodeByBound.h"
#include "algorithms/branch-and-bound/BaseNodeStrategy.h"
#include "algorithms/branch-and-cut-and-price/ColumnGenerator.h"
#include "algorithms/branch-and-cut-and-price/ColumnGenerationStrategy.h"
#include "algorithms/branch-and-bound/NodeByBoundStrategy.h"
#include "algorithms/branch-and-cut-and-price/DantzigWolfeGenerator.h"

#include "ex2_branch_and_price_gap/Instance.h"

int main() {

    Log::set_level(Trace);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);

    const Instance instance;

    const unsigned int n_knapsacks = instance.n_knapsacks;
    const unsigned int n_items = instance.n_items;
    const auto& p = instance.p;
    const auto& w = instance.w;
    const auto& c = instance.c;

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

    // Alg
    BranchAndBound solver;
    solver.set_node_strategy<NodeByBoundStrategy>();
    solver.set_branching_strategy<MostInfeasible>(branching_candidates);
    auto& generation_strategy = solver.set_solution_strategy<DecompositionStrategy<Lpsolve>>(rmp);
    auto& column_generation = generation_strategy.add_generation_strategy<ColumnGenerationStrategy>();

    // DantzigWolfe
    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        DantzigWolfeGenerator generator(rmp, subproblems[i]);
        column_generation.add_subproblem<ExternalSolverStrategy<Lpsolve>>(generator, subproblems[i]);
    }

    solver.solve();

    std::cout << solver.primal_solution() << std::endl;
    std::cout << "N. nodes: " << solver.n_created_nodes() << std::endl;

    return 0;
}

