//
// Created by henri on 10/09/22.
//
#include "../test_utils.h"

TEMPLATE_LIST_TEST_CASE("LP", "[LP][solvers]", available_solvers) {

    Env env;
    Model model(env);

    SECTION("solving LP") {
        // Example taken from http://lpsolve.sourceforge.net/5.5/formulate.htm#Construct%20the%20model%20from%20a%20Programming%20Language

        SECTION("Model constructed before the solver") {

            auto x = model.add_variable(0., Inf, Continuous, -143, "x");
            auto y = model.add_variable(0., Inf, Continuous, -60, "y");
            auto c1 = model.add_constraint(120 * x + 210 * y <= 15000);
            auto c2 = model.add_constraint(110 * x + 30 * y <= 4000);
            auto c3 = model.add_constraint(x + y <= 75);

            TestType solver(model);
            solver.solve();

            const auto solution = solver.solution(true, true, true);

            CHECK(solution.status() == Optimal);
            CHECK(solution.value(x) == 21.875_a);
            CHECK(solution.value(y) == 53.125_a);
            CHECK(solution.value(c1) == 0._a);
            CHECK(solution.value(c2) == -1.0375_a);
            CHECK(solution.value(c3) == -28.875_a);
            CHECK(solution.reduced_cost(x) == 0._a);
            CHECK(solution.reduced_cost(y) == 0._a);

        }

    }

}