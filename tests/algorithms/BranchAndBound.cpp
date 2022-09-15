//
// Created by henri on 15/09/22.
//

#include "../test_utils.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "algorithms/branch-and-bound/SolutionStrategy.h"
#include "algorithms/branch-and-bound/NodeStrategy.h"
#include "algorithms/branch-and-bound/MostInfeasible.h"
#include "algorithms/branch-and-bound/NodeByBound.h"

TEMPLATE_LIST_TEST_CASE("BranchAndBound", "[MILP][branch-and-bound][algorithms]", available_solvers) {

    Env env;
    Model model(env);

    SECTION("solving bounded feasible MILP") {

        SECTION("sovled at root node") {
            // Taken from https://www.gurobi.com/documentation/9.5/examples/mip1_cpp_cpp.html#subsubsection:mip1_c++.cpp

            auto x = model.add_variable(0., 1., Continuous, -1, "x");
            auto y = model.add_variable(0., 1., Continuous, -1, "y");
            auto z = model.add_variable(0., 1., Continuous, -2, "z");
            auto c1 = model.add_constraint(x + 2 * y + 3 * z <= 4);
            auto c2 = model.add_constraint(x + y >= 1);

            std::vector<Var> branching_candidates = { x, y, z };

            BranchAndBound solver;
            solver.set_solution_strategy(new SolutionStrategy<TestType>(model)); // how it is solved
            solver.set_node_strategy(new NodeStrategy<NodeByBound>()); // how it is stored
            solver.set_branching_strategy(new MostInfeasible(branching_candidates)); // how it is branched and checked for feasibility
            solver.solve();

            CHECK(solver.n_created_nodes() == 1);
            CHECK(solver.status() == Optimal);
            CHECK(solver.objective_value() == -3._a);

            const auto primal_solution = solver.primal_solution();

            CHECK(primal_solution.get(x) == 1._a);
            CHECK(primal_solution.get(y) == 0._a);
            CHECK(primal_solution.get(z) == 1._a);
        }

        SECTION("solved at fist level") {

            auto x = model.add_variable(0., 1., Continuous, -1, "x");
            auto y = model.add_variable(0., 1., Continuous, -1, "y");
            auto z = model.add_variable(0., 1., Continuous, -2, "z");
            auto c1 = model.add_constraint(x + 2 * y + 2.5 * z <= 4);
            auto c2 = model.add_constraint(x + y >= 1);

            std::vector<Var> branching_candidates = { x, y, z };

            BranchAndBound solver;
            solver.set_solution_strategy(new SolutionStrategy<TestType>(model)); // how it is solved
            solver.set_node_strategy(new NodeStrategy<NodeByBound>()); // how it is stored
            solver.set_branching_strategy(new MostInfeasible(branching_candidates)); // how it is branched and checked for feasibility
            solver.solve();

            CHECK(solver.n_created_nodes() == 3);
            CHECK(solver.status() == Optimal);
            CHECK(solver.objective_value() == -3._a);

            const auto primal_solution = solver.primal_solution();

            CHECK(primal_solution.get(x) == 1._a);
            CHECK(primal_solution.get(y) == 0._a);
            CHECK(primal_solution.get(z) == 1._a);
        }

    }

}