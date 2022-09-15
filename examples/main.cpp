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

    const std::vector<std::pair<double, double>> items = { {2, 40}, {3.14, 50}, {1.98, 100}, {5, 95}, {3, 30} }; // (weight, value)
    const double capacity = 10.;

    std::vector<Var> x;
    x.reserve(items.size());

    Expr sum_weight;

    for (const auto& [weight, profit] : items) {
        auto var = model.add_variable(0., 1., Continuous, -profit);
        sum_weight += weight * var;
        x.emplace_back(var);
    }

    model.add_constraint(sum_weight <= capacity);

    BranchAndBound solver;
    solver.set_solution_strategy(new SolutionStrategy<Gurobi>(model)); // how it is solved
    solver.set_node_strategy(new NodeStrategy<NodeByBound>()); // how it is stored
    solver.set_branching_strategy(new MostInfeasible(x)); // how it is branched and checked for feasibility
    solver.solve();

    std::cout << "Status: " << solver.status() << std::endl;
    std::cout << "Optimum: " << solver.objective_value() << std::endl;
    std::cout << "N. created nodes: " << solver.n_created_nodes() << std::endl;
    for (const auto& [var, val] : solver.primal_solution()) {
        std::cout << var << " = " << val << std::endl;
    }

    return 0;
}

