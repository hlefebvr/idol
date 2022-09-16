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

int main() {

    Log::set_level(Trace);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);

    Env env;

    Model rmp(env);
    auto x_bar_0 = rmp.add_variable(0., 10., Continuous, -1., "x_bar_0");
    auto x_bar_1 = rmp.add_variable(0., 10., Continuous, -1., "x_bar_1");
    auto ctr_rmp = rmp.add_constraint(-2. * x_bar_0 + 2. * x_bar_1 >= 1., "rmp_ctr");
    auto ctr_x_0 = rmp.add_constraint(-1. * x_bar_0 == 0., "x_bar_0");
    auto ctr_x_1 = rmp.add_constraint(-1. * x_bar_1 == 0., "x_bar_1");
    auto ctr_con = rmp.add_constraint(Equal, 1,            "convex_constraints");

    Model sp(env);
    auto x_0 = sp.add_variable(0., 10., Continuous, 0., "x_0");
    auto x_1 = sp.add_variable(0., 10., Continuous, 0., "x_1");
    auto sp_ctr = sp.add_constraint(-8 * x_0 + 10. * x_1 <= 13.);

    ColumnGenerator generator;
    generator.set(ctr_x_0, x_0);
    generator.set(ctr_x_1, x_1);
    generator.set(ctr_con, Expr(), 1.);

    BranchAndBound solver;
    solver.set_node_strategy<NodeByBoundStrategy>();
    solver.set_branching_strategy<MostInfeasible>(std::vector<Var> {  });

    auto& generation_strategy = solver.set_solution_strategy<DecompositionStrategy<Gurobi>>(rmp);
    auto& column_generation = generation_strategy.add_generation_strategy<ColumnGenerationStrategy>();
    auto& subproblem  = column_generation.add_subproblem<ExternalSolverStrategy<Lpsolve>>(generator, sp);

    solver.solve();

    std::cout << "Status: " << solver.status() << std::endl;
    std::cout << "Optimum: " << solver.objective_value() << std::endl;
    std::cout << "N. created nodes: " << solver.n_created_nodes() << std::endl;

    return 0;
}

