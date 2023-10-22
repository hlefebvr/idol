//
// Created by henri on 22.10.23.
//
#include <catch2/catch_all.hpp>
#include <idol/modeling.h>
#include <idol/optimizers/solvers/GLPK.h>
#include <idol/optimizers/solvers/Mosek.h>
#include <idol/optimizers/solvers/gurobi/Gurobi.h>
#include <idol/optimizers/solvers/HiGHS.h>

using namespace Catch::literals;
using namespace idol;

TEST_CASE("Solving small MIPs") {

    Env env;

    WHEN("A bounded and feasible MILP") {

        AND_WHEN("The root node is integer feasible") { // Taken from https://www.gurobi.com/documentation/9.5/examples/mip1_cpp_cpp.html#subsubsection:mip1_c++.cpp

            Var x(env, 0., 1., Binary, "x");
            Var y(env, 0., 1., Binary, "y");
            Var z(env, 0., 1., Binary, "z");
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

            THEN("The solution status should be Optimal") {
                CHECK(model.get_status() == Optimal);
            }

            AND_THEN("The objective value should be correct") {
                CHECK(model.get_best_obj() == -3_a);
            }

            AND_THEN("The returned solution values should be correct") {

                const auto primal_solution = save_primal(model);

                CHECK(primal_solution.get(x) == 1._a);
                CHECK(primal_solution.get(y) == 0._a);
                CHECK(primal_solution.get(z) == 1._a);

            }

        }

        AND_WHEN("The root not is not integer feasible") {


            Var x(env, 0., 1., Binary, "x");
            Var y(env, 0., 1., Binary, "y");
            Var z(env, 0., 1., Binary, "z");
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

            THEN("The solution status should be Optimal") {
                CHECK(model.get_status() == Optimal);
            }

            AND_THEN("The objective value should be correct") {
                CHECK(model.get_best_obj() == -3_a);
            }

            AND_THEN("The returned solution values should be correct") {

                const auto primal_solution = save_primal(model);

                CHECK(primal_solution.get(x) == 1._a);
                CHECK(primal_solution.get(y) == 0._a);
                CHECK(primal_solution.get(z) == 1._a);

            }


        }

    }

    AND_WHEN("An infeasible problem is solved") {

        WHEN("The root node is infeasible") {


            Var x(env, 0., 1., Binary, "x");
            Ctr c1(env, x >= 1);
            Ctr c2(env, x <= 0);

            Model model(env);
            model.add(x);
            model.add(c1);
            model.add(c2);

            model.use(OPTIMIZER());

            model.optimize();

            THEN("The solution status should be Infeasible") {

                CHECK(model.get_status() == Infeasible);

            }

            THEN("The objective value should be +inf") {

                CHECK(is_pos_inf(model.get_best_obj()));

            }

        }

        AND_WHEN("The root node is feasible") {

            Var x(env, 0., 1., Binary, "x");
            Ctr c1(env, x >= .1);
            Ctr c2(env, x <= .9);

            Model model(env);

            model.add(x);
            model.add(c1);
            model.add(c2);

            model.use(OPTIMIZER());

            model.optimize();

            THEN("The solution status should be Infeasible") {

                CHECK(model.get_status() == Infeasible);

            }

            THEN("The objective value should be +inf") {

                CHECK(is_pos_inf(model.get_best_obj()));

            }

        }

    }

    AND_WHEN("An unbounded MILP is solved") {

        Var x(env, -Inf, Inf, Integer, "x");
        Model model(env);
        model.add(x);
        model.set_obj_expr(-x);

        model.use(OPTIMIZER());

        model.optimize();

        THEN("The solution status should be Unbounded") {

            const auto status = model.get_status();

            const bool unknown_or_unbounded = status == Unbounded || status == InfOrUnbnd;

            CHECK(unknown_or_unbounded);

        }


    }


}
