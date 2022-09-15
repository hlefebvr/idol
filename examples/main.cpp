#include <iostream>
#include "modeling.h"
#include "solvers/gurobi/Gurobi.h"
#include "solvers/lpsolve/Lpsolve.h"
#include "algorithms/column-generation/ColGenModel.h"
#include "algorithms/column-generation/ColGenerator.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "algorithms/branch-and-bound/NodeByBound.h"
#include "algorithms/branch-and-bound/SolutionStrategy.h"
#include "algorithms/branch-and-bound/NodeStrategy.h"
#include "algorithms/branch-and-bound/MostInfeasible.h"
#include "algorithms/logs/Log.h"

int main() {

    Log::set_level(Info);

    Env env;
    Model model(env);
    auto x = model.add_variable(0., 1., Continuous, -1, "x");
    auto y = model.add_variable(0., 1., Continuous, -1, "y");
    auto z = model.add_variable(0., 1., Continuous, -2, "z");
    auto c1 = model.add_constraint(x + 2 * y + 2.5 * z <= 4);
    auto c2 = model.add_constraint(x + y >= 1);

    std::vector<Var> branching_candidates = { x, y, z };

    BranchAndBound solver;
    solver.set_solution_strategy(new SolutionStrategy<Gurobi>(model)); // how it is solved
    solver.set_node_strategy(new NodeStrategy<NodeByBound>()); // how it is stored
    solver.set_branching_strategy(new MostInfeasible(branching_candidates)); // how it is branched and checked for feasibility
    solver.solve();

    std::cout << "Status: " << solver.status() << std::endl;
    std::cout << "Optimum: " << solver.objective_value() << std::endl;

    return 0;
}

