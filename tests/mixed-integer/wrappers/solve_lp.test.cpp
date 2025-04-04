//
// Created by henri on 22.10.23.
//
#include <catch2/catch_all.hpp>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Mosek/Mosek.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/Osi/Osi.h"

using namespace Catch::literals;
using namespace idol;

TEST_CASE("Can solve a feasible LP", "[solving-lp]") {

    // Example taken from http://lpsolve.sourceforge.net/5.5/formulate.htm#Construct%20the%20model%20from%20a%20Programming%20Language

    Env env;

    Var x(env, 0, Inf, Continuous, 0., "x");
    Var y(env, 0, Inf, Continuous, 0., "y");

    Ctr c1(env, 120 * x + 210 * y <= 15000);
    Ctr c2(env, 110 * x +  30 * y <=  4000);
    Ctr c3(env,       x +       y <=    75);

    Model model(env);
    model.add(x);
    model.add(y);
    model.add(c1);
    model.add(c2);
    model.add(c3);
    model.set_obj_expr(-143 * x - 60 * y);

    model.use(OPTIMIZER());

    model.optimize();

    SECTION("Can retrieve primal solution") {

        CHECK(model.get_status() == Optimal);
        CHECK(model.get_best_obj() == -6315.625_a);
        CHECK(model.get_var_primal(x) == 21.875_a);
        CHECK(model.get_var_primal(y) == 53.125_a);

        const auto primal_solution = save_primal(model);
        CHECK(primal_solution.status() == Optimal);
        CHECK(primal_solution.get(x) == 21.875_a);
        CHECK(primal_solution.get(y) == 53.125_a);

    }

    SECTION("Can retrieve dual solution") {

        CHECK(model.get_status() == Optimal);
        CHECK(model.get_best_bound() == -6315.625_a);
        CHECK(model.get_ctr_dual(c1) == 0_a);
        CHECK(model.get_ctr_dual(c2) == -1.0375_a);
        CHECK(model.get_ctr_dual(c3) == -28.875_a);

        const auto dual_solution = save_dual(model);

        CHECK(dual_solution.status() == Optimal);
        CHECK(dual_solution.get(c1) == 0._a);
        CHECK(dual_solution.get(c2) == -1.0375_a);
        CHECK(dual_solution.get(c3) == -28.875_a);

    }

    SECTION("Can retrieve reduced costs") {

        CHECK(model.get_var_reduced_cost(x) == 0._a);
        CHECK(model.get_var_reduced_cost(y) == 0._a);

        const auto reduced_costs = save_reduced_cost(model);
        CHECK(reduced_costs.get(x) == 0._a);
        CHECK(reduced_costs.get(y) == 0._a);

    }

    SECTION("Can retrieve basis") {
        CHECK(false);
    }

    SECTION("Throws an exception if primal ray is asked") {
        CHECK_THROWS(model.get_var_ray(x));
        CHECK_THROWS(save_ray(model));
    }

    SECTION("Throws an exception if farkas certificate is asked") {
        CHECK_THROWS(model.get_ctr_farkas(c1));
        CHECK_THROWS(save_farkas(model));
    }

}

TEST_CASE("Can update and re-optimize a feasible LP", "[solving-lp]") {

    SECTION("Can add cut and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add column and re-optimize") {
        CHECK(false);
    }

    SECTION("Can change bounds and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add cut and column and re-optimize") {
        CHECK(false);
    }

    SECTION("Can change objective function and re-optimize") {
        CHECK(false);
    }

    SECTION("Can change right-hand side and re-optimize") {
        CHECK(false);
    }

    SECTION("Can change left-hand side and re-optimize") {
        CHECK(false);
    }

    SECTION("Can remove a constraint and re-optimize") {
        CHECK(false);
    }

    SECTION("Can remove a variable and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add and remove the same variable and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add and remove a different variable and re-optimize") {
        CHECK(false);
    }

    SECTION("Can add and remove the same constraint") {
        CHECK(false);
    }

    SECTION("Can add and remove a different constraint") {
        CHECK(false);
    }

    SECTION("Can add a constraint which depends on a variable that is then removed") {
        CHECK(false);
    }

    SECTION("Can add a variable which depends on a constraint that is then removed") {
        CHECK(false);
    }

    SECTION("Can update a variable's bound then remove it") {
        CHECK(false);
    }

    SECTION("Can write a model to a file and read it back") {
        CHECK(false);
    }

    SECTION("Can change the objective function then remove a variable which appears in the objective function") {
        CHECK(false);
    }

    SECTION("Can update the objective function constant") {
        CHECK(false);
    }

}

TEST_CASE("Can solve an infeasible LP", "[solving-lp]") {

    Env env;

    Var u(env, 0., Inf, Continuous, 0., "u");
    Var v(env, 0., Inf, Continuous, 0., "v");
    Var w(env, 0., Inf, Continuous, 0., "w");
    Ctr c1(env, u + -2 * v + -1 * w >= 3);
    Ctr c2(env, -2 * u + v + -1 * w >= 2);
    auto objective = u + v - 2 * w;

    Model model(env);
    model.add(u);
    model.add(v);
    model.add(w);
    model.add(c1);
    model.add(c2);
    model.set_obj_expr(objective);

    model.use(OPTIMIZER().with_infeasible_or_unbounded_info(true));

    model.optimize();

    SECTION("Can retrieve a Farkas certificate") {

        CHECK(model.get_status() == Infeasible);
        CHECK(model.get_best_obj() == Inf);

        const auto farkas = save_farkas(model);
        CHECK(farkas.status() == Infeasible);
        const double c1_val = farkas.get(c1);
        const double c2_val = farkas.get(c2);
        CHECK(1. * c1_val - 2. * c2_val <= 0. );
        CHECK(-2. * c1_val + 1. * c2_val <= 0. );
        CHECK(-1. * c1_val + -1. * c2_val <= 0. );

    }

    SECTION("Throws an exception if primal values are asked") {
        CHECK_THROWS(model.get_var_primal(u));
        CHECK_THROWS(save_primal(model));
    }

    SECTION("Throws an exception if dual values are asked") {
        CHECK_THROWS(model.get_ctr_dual(c1));
        CHECK_THROWS(save_dual(model));
    }

    SECTION("Throws an exception if (primal) ray is asked") {
        CHECK_THROWS(model.get_var_ray(u));
        CHECK_THROWS(save_ray(model));
    }

    SECTION("Throws an exception if reduced costs are asked") {
        CHECK_THROWS(model.get_var_reduced_cost(u));
        CHECK_THROWS(save_reduced_cost(model));
    }

}

TEST_CASE("Can solve an unbounded LP", "[solving-lp]") {

    Env env;

    Var x(env, 0, Inf, Continuous, 0., "x");
    Var y(env, 0, Inf, Continuous, 0., "y");

    Ctr c1(env, x - 2 * y <= 1);
    Ctr c2(env, -2 * x + y <= 1);
    Ctr c3(env, x + y >= 2);

    Model model(env);
    model.add(x);
    model.add(y);
    model.add(c1);
    model.add(c2);
    model.add(c3);
    model.set_obj_expr(-3 * x - 2 * y);

    model.use(OPTIMIZER().with_infeasible_or_unbounded_info(true));

    model.optimize();

    SECTION("Can retrieve a dual ray") {

        CHECK(model.get_status() == Unbounded);
        CHECK(model.get_best_obj() == -Inf);

        const auto dual_ray = save_ray(model);
        CHECK(dual_ray.status() == Unbounded);

        const double x_val = dual_ray.get(x);
        const double y_val = dual_ray.get(y);

        CHECK(-3. * x_val - 2. * y_val <= 0._a);
        CHECK(-2. * x_val + y_val <= 0. + 1e-5);
        CHECK(x_val + y_val >= 0_a);

    }

    SECTION("Throws an exception if primal values are asked") {
        CHECK_THROWS(model.get_var_primal(x));
        CHECK_THROWS(save_primal(model));
    }

    SECTION("Throws an exception if dual values are asked") {
        CHECK_THROWS(model.get_ctr_dual(c1));
        CHECK_THROWS(save_dual(model));
    }

    SECTION("Throws an exception if farkas certificate is asked") {
        CHECK_THROWS(model.get_ctr_farkas(c1));
        CHECK_THROWS(save_farkas(model));
    }

    SECTION("Throws an exception if reduced costs are asked") {
        CHECK_THROWS(model.get_var_reduced_cost(x));
        CHECK_THROWS(save_reduced_cost(model));
    }

}
