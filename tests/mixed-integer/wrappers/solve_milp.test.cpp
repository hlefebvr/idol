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

TEST_CASE("Can solve a feasible MIP which is integer at the root node", "[solving-milp]") {

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

    SECTION("Can retrieve the number of solutions") {
        CHECK(false);
    }

    SECTION("Can iterate over solutions") {
        CHECK(false);
    }

}


TEST_CASE("Can solve a feasible MIP which is not integer at the root node", "[solving-milp]") {

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

    SECTION("Can retrieve the number of solutions") {
        CHECK(false);
    }

    SECTION("Can iterate over solutions") {
        CHECK(false);
    }

}

TEST_CASE("Can solve an infeasible MIP which is infeasible at root node", "[solving-milp]") {

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

    SECTION("Can retrieve the solution status") {
        CHECK(model.get_status() == Infeasible);
        CHECK(is_pos_inf(model.get_best_obj()));
    }

    SECTION("Can compute an irreducible infeasible sub-system") {
        CHECK(false);
    }

}

TEST_CASE("Can solve an infeasible MIP which is feasible at the root node", "[solving-milp]") {

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

    SECTION("Can retrieve the solution status", "[solving-milp]") {
        CHECK(model.get_status() == Infeasible);
        CHECK(is_pos_inf(model.get_best_obj()));
    }

    SECTION("Can compute an irreducible infeasible sub-system") {
        CHECK(false);
    }

}

TEST_CASE("Can solve an unbounded MIP", "[solving-milp]") {

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

TEST_CASE("Can set parameters of the underlying solver", "[solving-milp]") {

    SECTION("Can set a time limit") {
        CHECK(false);
    }

    SECTION("Can turn off presolve") {
        CHECK(false);
    }

    SECTION("Can activate infeasible or unbounded info") {
        CHECK(false);
    }

    SECTION("Can set an external parameter") {
        CHECK(false);
    }

    SECTION("Can set a thread limit") {
        CHECK(false);
    }

    SECTION("Can set a relative MIP gap") {
        CHECK(false);
    }

    SECTION("Can set an absolute MIP gap") {
        CHECK(false);
    }

    SECTION("Can set a best bound stop") {
        CHECK(false);
    }

    SECTION("Can set a best obj stop") {
        CHECK(false);
    }

    SECTION("Can set an iteration limit") {
        CHECK(false);
    }

    SECTION("Can turn on logs") {
        CHECK(false);
    }

    SECTION("Can set the maximum number of solutions in the pool") {
        CHECK(false);
    }

}
