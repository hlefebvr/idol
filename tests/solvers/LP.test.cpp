//
// Created by henri on 10/09/22.
//
#include "../test_utils.h"

TEMPLATE_LIST_TEST_CASE("LP", "[LP][solvers]", for_each_solver) {

    Env env;
    Model model(env);

    auto x = model.add_variable(0., 1., Binary, -1, "x");
    auto y = model.add_variable(0., 1., Binary, -2, "y");

    auto ctr = model.add_constraint(x + y <= 1);

    TestType solver(model);
    solver.solve();

}