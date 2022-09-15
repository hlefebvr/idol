#include <iostream>
#include "modeling.h"
#include "solvers/gurobi/Gurobi.h"
#include "solvers/lpsolve/Lpsolve.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "algorithms/logs/Log.h"
#include "algorithms/branch-and-cut-and-price/BranchAndCutAndPriceStrategy.h"
#include "algorithms/branch-and-bound/MostInfeasible.h"
#include "algorithms/branch-and-bound/NodeByBound.h"
#include "algorithms/branch-and-bound/NodeStrategy.h"
#include "algorithms/branch-and-cut-and-price/ColumnGenerator.h"

int main() {

    Log::set_level(Info);

    Env env;


    Model rmp(env);
    auto x_bar_0 = rmp.add_variable(0., 10., Continuous, -1.);
    auto x_bar_1 = rmp.add_variable(0., 10., Continuous, -1.);
    auto ctr_rmp = rmp.add_constraint(-2. * x_bar_0 + 2. * x_bar_1 >= 1.);
    auto ctr_x_0 = rmp.add_constraint(x_bar_0 == 0., "x_bar_0");
    auto ctr_x_1 = rmp.add_constraint(x_bar_1 == 0., "x_bar_1");
    auto ctr_con = rmp.add_constraint(Equal, 1.,     "convex_constraints");

    Model sp(env);
    auto x_0 = sp.add_variable(0., 10., Continuous, 0.);
    auto x_1 = sp.add_variable(0., 10., Continuous, 0.);
    auto sp_ctr = sp.add_constraint(-8 * x_0 + 10. * x_1 <= 13.);

    ColumnGenerator generator;
    generator.set(ctr_x_0, x_0);
    generator.set(ctr_x_1, x_1);
    generator.set(ctr_con, Expr(), 1.);

    auto* strategy = new BranchAndCutAndPriceStrategy<Lpsolve>(rmp);

    BranchAndBound solver;
    solver.set_solution_strategy(strategy);
    solver.set_node_strategy(new NodeStrategy<NodeByBound>());
    solver.set_branching_strategy(new MostInfeasible({ x_bar_0, x_bar_1 }));
    solver.solve();

    std::cout << "Status: " << solver.status() << std::endl;
    std::cout << "Optimum: " << solver.objective_value() << std::endl;
    std::cout << "N. created nodes: " << solver.n_created_nodes() << std::endl;

    return 0;
}

