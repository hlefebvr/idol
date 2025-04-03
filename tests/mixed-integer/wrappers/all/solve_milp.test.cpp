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

/*
 * - Test parameters
 * - IIS?
 */

TEST_CASE("Can solve a feasible MIP which is integer at the root node", "[mip-solving]") {

    // Taken from https://www.gurobi.com/documentation/9.5/examples/mip1_cpp_cpp.html#subsubsection:mip1_c++.cpp
    Env env;

    Var x(env, 0., 1., Binary, 0., "x");
    Var y(env, 0., 1., Binary, 0., "y");
    Var z(env, 0., 1., Binary, 0., "z");
    Ctr c1(env, x + 2 * y + 3 * z <= 4);
    Ctr c2(env, x + y >= 1);
    auto objective = -x - y - 2 * z;

    Model model(env);
    model.add(x);
    model.add(y);
    model.add(z);
    model.add(c1);
    model.add(c2);
    model.set_obj_expr(objective);

    model.use(OPTIMIZER());

    model.optimize();

    SECTION("Can retrieve primal solution") {
        CHECK(model.get_status() == Optimal);
        CHECK(model.get_best_obj() == -3_a);

        const auto primal_solution = save_primal(model);

        CHECK(primal_solution.get(x) == 1._a);
        CHECK(primal_solution.get(y) == 0._a);
        CHECK(primal_solution.get(z) == 1._a);

    }

}


TEST_CASE("Can solve a feasible MIP which is not integer at the root node", "[mip-solving]") {

    Env env;

    Var x(env, 0., 1., Binary, 0., "x");
    Var y(env, 0., 1., Binary, 0., "y");
    Var z(env, 0., 1., Binary, 0., "z");
    Ctr c1(env, x + 2 * y + 2.5 * z <= 4);
    Ctr c2(env, x + y >= 1);
    auto objective = -x - y - 2 * z;

    Model model(env);
    model.add(x);
    model.add(y);
    model.add(z);
    model.add(c1);
    model.add(c2);
    model.set_obj_expr(objective);

    model.use(OPTIMIZER());

    model.optimize();

    SECTION("Can retrieve the primal solution") {
        CHECK(model.get_status() == Optimal);
        CHECK(model.get_best_obj() == -3_a);

        const auto primal_solution = save_primal(model);

        CHECK(primal_solution.get(x) == 1._a);
        CHECK(primal_solution.get(y) == 0._a);
        CHECK(primal_solution.get(z) == 1._a);

    }

}

TEST_CASE("Can solve an infeasible MIP which is infeasible at root node", "[mip-solving]") {

    Env env;

    Var x(env, 0., 1., Binary, 0., "x");
    Ctr c1(env, x >= 1);
    Ctr c2(env, x <= 0);

    Model model(env);
    model.add(x);
    model.add(c1);
    model.add(c2);

    model.use(OPTIMIZER());

    model.optimize();

    THEN("Can retrieve the solution status") {
        CHECK(model.get_status() == Infeasible);
        CHECK(is_pos_inf(model.get_best_obj()));
    }

}

TEST_CASE("Can solve an infeasible MIP which is feasible at the root node", "[mip-solving]") {

    Env env;

    Var x(env, 0., 1., Binary, 0., "x");
    Ctr c1(env, x >= .1);
    Ctr c2(env, x <= .9);

    Model model(env);

    model.add(x);
    model.add(c1);
    model.add(c2);

    model.use(OPTIMIZER());

    model.optimize();

    SECTION("Can retrieve the solution status", "[mip-solving]") {
        CHECK(model.get_status() == Infeasible);
        CHECK(is_pos_inf(model.get_best_obj()));
    }

}

TEST_CASE("Can solve an unbounded MIP", "[mip-solving]") {
    Env env;
    Var x(env, -Inf, Inf, Integer, 0., "x");
    Model model(env);
    model.add(x);
    model.set_obj_expr(-x);

    model.use(OPTIMIZER());

    model.optimize();

    SECTION("Can retrieve the solution status") {
        CHECK(model.get_status() == Unbounded);
        CHECK(is_neg_inf(model.get_best_obj()));
    }
}
