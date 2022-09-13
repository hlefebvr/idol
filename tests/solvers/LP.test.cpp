//
// Created by henri on 10/09/22.
//
#include "../test_utils.h"

TEMPLATE_LIST_TEST_CASE("LP", "[LP][solvers]", available_solvers) {

    Env env;
    Model model(env);

    SECTION("solving bounded feasible LP") {
        // Example taken from http://lpsolve.sourceforge.net/5.5/formulate.htm#Construct%20the%20model%20from%20a%20Programming%20Language

        SECTION("Model constructed before the solver is declared") {

            auto x = model.add_variable(0., Inf, Continuous, -143, "x");
            auto y = model.add_variable(0., Inf, Continuous, -60, "y");

            auto c1 = model.add_constraint(120 * x + 210 * y <= 15000);
            auto c2 = model.add_constraint(110 * x + 30 * y <= 4000);
            auto c3 = model.add_constraint(x + y <= 75);

            TestType solver(model);
            solver.solve();

            const auto primal_solution = solver.primal_solution();
            const auto dual_solution = solver.dual_solution();

            CHECK(primal_solution.status() == Optimal);
            CHECK(primal_solution.get(x) == 21.875_a);
            CHECK(primal_solution.get(y) == 53.125_a);

            CHECK(dual_solution.status() == Optimal);
            CHECK(dual_solution.get(c1) == 0._a);
            CHECK(dual_solution.get(c2) == -1.0375_a);
            CHECK(dual_solution.get(c3) == -28.875_a);

        }

        SECTION("Model constructed after the solver is declared") {

            TestType solver(model);

            auto x = model.add_variable(0., Inf, Continuous, -143, "x");
            auto y = model.add_variable(0., Inf, Continuous, -60, "y");
            auto c1 = model.add_constraint(120 * x + 210 * y <= 15000);
            auto c2 = model.add_constraint(110 * x + 30 * y <= 4000);
            auto c3 = model.add_constraint(x + y <= 75);

            solver.solve();

            const auto primal_solution = solver.primal_solution();
            const auto dual_solution = solver.dual_solution();

            CHECK(primal_solution.status() == Optimal);
            CHECK(primal_solution.get(x) == 21.875_a);
            CHECK(primal_solution.get(y) == 53.125_a);

            CHECK(dual_solution.status() == Optimal);
            CHECK(dual_solution.get(c1) == 0._a);
            CHECK(dual_solution.get(c2) == -1.0375_a);
            CHECK(dual_solution.get(c3) == -28.875_a);

        }

    }

    SECTION("Solving unbounded LP") {

        auto x = model.add_variable(0., Inf, Continuous, -3, "x");
        auto y = model.add_variable(0., Inf, Continuous, -2, "y");
        auto c1 = model.add_constraint(x + -2 * y <= 1);
        auto c2 = model.add_constraint(-2 * x + y <= 1);
        auto c3 = model.add_constraint(x + y >= 2);

        TestType solver(model);
        solver.set_infeasible_or_unbounded_info(true);
        solver.solve();

        const auto primal_solution = solver.primal_solution();
        const auto dual_solution = solver.dual_solution();
        const auto extreme_ray = solver.extreme_ray().normalize(Inf);

        CHECK(primal_solution.status() == Unbounded);
        CHECK(is_neg_inf(primal_solution.objective_value()));

        CHECK(dual_solution.status() == Infeasible);
        CHECK(is_neg_inf(dual_solution.objective_value()));

        CHECK(extreme_ray.objective_value() == 0._a);
        CHECK(extreme_ray.get(x) == 1._a);
        CHECK(extreme_ray.get(y) == .5_a);

    }

    SECTION("Solving infeasible LP") {

        auto u = model.add_variable(0., Inf, Continuous, 1, "u");
        auto v = model.add_variable(0., Inf, Continuous, 1, "v");
        auto w = model.add_variable(0., Inf, Continuous, -2, "w");
        auto c1 = model.add_constraint(u + -2 * v + -1 * w >= 3);
        auto c2 = model.add_constraint(-2 * u + v + -1 * w >= 2);

        TestType solver(model);
        solver.set_infeasible_or_unbounded_info(true);
        solver.solve();

        const auto primal_solution = solver.primal_solution();
        const auto dual_solution = solver.dual_solution();
        const auto farkas = solver.dual_farkas().normalize(Inf);

        CHECK(primal_solution.status() == Infeasible);
        CHECK(dual_solution.status() == Unbounded);
        CHECK(farkas.objective_value() == 5._a);
        CHECK(farkas.get(c1) == 1._a);
        CHECK(farkas.get(c2) == 1._a);

    }

}