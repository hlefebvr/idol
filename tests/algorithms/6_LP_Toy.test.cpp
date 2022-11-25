//
// Created by henri on 10/09/22.
//
#include "../test_utils.h"

TEMPLATE_LIST_TEST_CASE("06. LP: Toy", has_lp_solver ? "[LP][solvers]" : "[.]", lp_solvers) {

    Model model;

    const bool is_minimization = GENERATE(true, false);
    const double sign = is_minimization ? 1. : -1.;

    SECTION("solving bounded feasible LP") {
        // Example taken from http://lpsolve.sourceforge.net/5.5/formulate.htm#Construct%20the%20model%20from%20a%20Programming%20Language

        SECTION("Model constructed before the solver is declared") {

            auto x = model.add_var(0., Inf, Continuous, sign * -143, "x");
            auto y = model.add_var(0., Inf, Continuous, sign * -60, "y");

            model.update_obj_sense(is_minimization ? Minimize : Maximize);

            auto c1 = model.add_ctr(120 * x + 210 * y <= 15000);
            auto c2 = model.add_ctr(110 * x + 30 * y <= 4000);
            auto c3 = model.add_ctr(x + y <= 75);

            TestType solver(model);
            solver.solve();

            const auto primal_solution = solver.primal_solution();
            const auto dual_solution = solver.dual_solution();

            CHECK(primal_solution.status() == Optimal);
            CHECK(primal_solution.get(x) == 21.875_a);
            CHECK(primal_solution.get(y) == 53.125_a);

            CHECK(dual_solution.status() == Optimal);
            CHECK(dual_solution.get(c1) == 0._a);
            CHECK(dual_solution.get(c2) == Catch::Approx(sign * -1.0375));
            CHECK(dual_solution.get(c3) == Catch::Approx(sign * -28.875));

        }

    }

    SECTION("Solving unbounded LP") {

        auto x = model.add_var(0., Inf, Continuous, -3, "x");
        auto y = model.add_var(0., Inf, Continuous, -2, "y");
        auto c1 = model.add_ctr(x + -2 * y <= 1);
        auto c2 = model.add_ctr(-2 * x + y <= 1);
        auto c3 = model.add_ctr(x + y >= 2);

        TestType solver(model);
        solver.set(Param::Algorithm::InfeasibleOrUnboundedInfo, true);
        solver.solve();

        const auto primal_solution = solver.primal_solution();
        const auto dual_solution = solver.dual_solution();
        const auto extreme_ray = solver.unbounded_ray();

        CHECK(primal_solution.status() == Unbounded);
        CHECK(is_neg_inf(primal_solution.objective_value()));

        CHECK(dual_solution.status() == Infeasible);
        CHECK(is_neg_inf(dual_solution.objective_value()));

        CHECK(extreme_ray.objective_value() <= 0_a);
        CHECK(-3. * extreme_ray.get(x) -2. * extreme_ray.get(y) <= 0._a);
        CHECK(-2. * extreme_ray.get(x) + extreme_ray.get(y) <= 0._a);
        CHECK(extreme_ray.get(x) + extreme_ray.get(y) >= 0_a);

    }

    SECTION("Solving infeasible LP") {

        auto u = model.add_var(0., Inf, Continuous, 1, "u");
        auto v = model.add_var(0., Inf, Continuous, 1, "v");
        auto w = model.add_var(0., Inf, Continuous, -2, "w");
        auto c1 = model.add_ctr(u + -2 * v + -1 * w >= 3);
        auto c2 = model.add_ctr(-2 * u + v + -1 * w >= 2);

        TestType solver(model);
        solver.set(Param::Algorithm::InfeasibleOrUnboundedInfo, true);
        solver.solve();

        const auto primal_solution = solver.primal_solution();
        const auto dual_solution = solver.dual_solution();
        const auto farkas = solver.farkas_certificate().normalize(Inf);

        CHECK(primal_solution.status() == Infeasible);
        CHECK(dual_solution.status() == Unbounded);


        CHECK(farkas.objective_value() > 0.);
        CHECK(1. * farkas.get(c1) - 2. * farkas.get(c2) <= 0. );
        CHECK(-2. * farkas.get(c1) + 1. * farkas.get(c2) <= 0. );
        CHECK(-1. * farkas.get(c1) + -1. * farkas.get(c2) <= 0. );

    }

}