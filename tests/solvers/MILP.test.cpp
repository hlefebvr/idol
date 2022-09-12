//
// Created by henri on 12/09/22.
//
#include "../test_utils.h"

TEMPLATE_LIST_TEST_CASE("MILP", "[MILP][solvers]", available_solvers) {

    Env env;
    Model model(env);

    SECTION("solving MILP") {
        // Example taken from https://www.gurobi.com/documentation/9.5/examples/mip1_cpp_cpp.html#subsubsection:mip1_c++.cpp

        SECTION("Model constructed before the solver is declared") {

            auto x = model.add_variable(0., 1., Binary, -1, "x");
            auto y = model.add_variable(0., 1., Binary, -1, "y");
            auto z = model.add_variable(0., 1., Binary, -2, "z");
            auto c1 = model.add_constraint(x + 2 * y + 3 * z <= 4);
            auto c2 = model.add_constraint(x + y >= 1);

            TestType solver(model);
            solver.solve();

            const auto solution = solver.solution();

            CHECK(solution.status() == Optimal);
            CHECK(solution.value(x) == 1._a);
            CHECK(solution.value(y) == 0._a);
            CHECK(solution.value(z) == 1._a);

        }

        SECTION("Model constructed after the solver is declared") {

            TestType solver(model);

            auto x = model.add_variable(0., 1., Binary, -1, "x");
            auto y = model.add_variable(0., 1., Binary, -1, "y");
            auto z = model.add_variable(0., 1., Binary, -2, "z");
            auto c1 = model.add_constraint(x + 2 * y + 3 * z <= 4);
            auto c2 = model.add_constraint(x + y >= 1);

            solver.solve();

            const auto solution = solver.solution();

            CHECK(solution.status() == Optimal);
            CHECK(solution.value(x) == 1._a);
            CHECK(solution.value(y) == 0._a);
            CHECK(solution.value(z) == 1._a);

        }

    }

}