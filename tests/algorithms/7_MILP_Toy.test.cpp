//
// Created by henri on 12/09/22.
//
#include "../test_utils.h"


TEMPLATE_LIST_TEST_CASE("07. MILP: Toy", has_milp_solver ? "[MILP][solvers]" : "[.]", milp_solvers) {

    Model model;

    SECTION("solving MILP") {
        // Example taken from https://www.gurobi.com/documentation/9.5/examples/mip1_cpp_cpp.html#subsubsection:mip1_c++.cpp

        SECTION("Model constructed before the solver is declared") {

            auto x = model.add_var(0., 1., Binary, -1, "x");
            auto y = model.add_var(0., 1., Binary, -1, "y");
            auto z = model.add_var(0., 1., Binary, -2, "z");
            auto c1 = model.add_ctr(x + 2 * y + 3 * z <= 4);
            auto c2 = model.add_ctr(x + y >= 1);

            TestType solver(model);
            solver.solve();

            const auto primal_solution = solver.primal_solution();

            CHECK(primal_solution.status() == Optimal);
            CHECK(primal_solution.get(x) == 1._a);
            CHECK(primal_solution.get(y) == 0._a);
            CHECK(primal_solution.get(z) == 1._a);

        }

    }

}