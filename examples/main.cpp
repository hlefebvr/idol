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

    Log::set_level(Trace);

    Env env;
    Model model(env);

    auto x = model.add_variable(-Inf, Inf, Continuous, -1.);

    BranchAndBound solver;
    solver.set_solution_strategy(new SolutionStrategy<Gurobi>(model)); // how it is solved
    solver.set_node_strategy(new NodeStrategy<NodeByBound>()); // how it is stored
    solver.set_branching_strategy(new MostInfeasible({ x })); // how it is branched and checked for feasibility
    solver.solve();

    std::cout << "Status: " << solver.status() << std::endl;
    std::cout << "Optimum: " << solver.objective_value() << std::endl;
    std::cout << "N. created nodes: " << solver.n_created_nodes() << std::endl;
    /* for (const auto& [var, val] : solver.primal_solution()) {
        std::cout << var << " = " << val << std::endl;
    } */

    return 0;
}

