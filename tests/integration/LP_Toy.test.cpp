//
// Created by henri on 06/02/23.
//

#include "../test_utils.h"
#include "optimizers/solvers/DefaultOptimizer.h"

TEMPLATE_LIST_TEST_CASE("LP solvers: solve toy example",
                        "[integration][backend][solver]",
                        lp_solvers) {

    Env env;

    WHEN("A bounded and feasible LP is solved") { // Example taken from http://lpsolve.sourceforge.net/5.5/formulate.htm#Construct%20the%20model%20from%20a%20Programming%20Language

        Var x(env, 0, Inf, Continuous, "x");
        Var y(env, 0, Inf, Continuous, "y");

        Ctr c1(env, 120 * x + 210 * y <= 15000);
        Ctr c2(env, 110 * x +  30 * y <=  4000);
        Ctr c3(env,       x +       y <=    75);

        Model model(env);
        model.add(x);
        model.add(y);
        model.add(c1);
        model.add(c2);
        model.add(c3);
        model.set_obj(-143 * x - 60 * y);

        model.use(TestType());

        model.optimize();

        THEN("The solution status should be Optimal") {

            CHECK(model.get_status() == Optimal);

        }

        AND_THEN("The objective value should be correct") {

            CHECK(model.get_best_obj() == -6315.625_a);

        }

        AND_THEN("The primal values should be correct") {

            const auto primal_solution = save_primal_values(model);

            CHECK(primal_solution.status() == Optimal);
            CHECK(primal_solution.get(x) == 21.875_a);
            CHECK(primal_solution.get(y) == 53.125_a);

        }

        AND_THEN("The dual values should be correct") {

            const auto dual_solution = save_dual_values(model);

            CHECK(dual_solution.status() == Optimal);
            CHECK(dual_solution.get(c1) == 0._a);
            CHECK(dual_solution.get(c2) == Catch::Approx(-1.0375));
            CHECK(dual_solution.get(c3) == Catch::Approx(-28.875));

        }

    }

    WHEN("An unbounded LP is solved") {

        Var x(env, 0, Inf, Continuous, "x");
        Var y(env, 0, Inf, Continuous, "y");

        Ctr c1(env, x - 2 * y <= 1);
        Ctr c2(env, -2 * x + y <= 1);
        Ctr c3(env, x + y >= 2);

        Model model(env);
        model.add(x);
        model.add(y);
        model.add(c1);
        model.add(c2);
        model.add(c3);
        model.set_obj(-3 * x - 2 * y);

        model.use(TestType().with_infeasible_or_unbounded_info(true));

        model.optimize();

        THEN("The solution status should be Unbounded") {

            CHECK(model.get_status() == Unbounded);

        }

        AND_THEN("The objective value should be -Inf") {

            CHECK(is_neg_inf(model.get_best_obj()));

        }

        AND_THEN("The unbounded ray should be valid") {

            const auto ray = save_ray_values(model);
            
            const double x_val = ray.get(x);
            const double y_val = ray.get(y);
            
            CHECK(-3. * x_val -2. * y_val <= 0._a);
            CHECK(-2. * x_val + y_val <= 0._a);
            CHECK(x_val + y_val >= 0_a);

        }

    }
    
    WHEN("An infeasible LP is solved") {

        Var u(env, 0., Inf, Continuous, "u");
        Var v(env, 0., Inf, Continuous, "v");
        Var w(env, 0., Inf, Continuous, "w");
        Ctr c1(env, u + -2 * v + -1 * w >= 3);
        Ctr c2(env, -2 * u + v + -1 * w >= 2);
        auto objective = u + v - 2 * w;

        Model model(env);
        model.add(u);
        model.add(v);
        model.add(w);
        model.add(c1);
        model.add(c2);
        model.set_obj(objective);

        model.use(TestType().with_infeasible_or_unbounded_info(true));

        model.optimize();


        THEN("The solution status should be Infeasible") {

            CHECK(model.get_status() == Infeasible);

        }

        AND_THEN("The objective value should be -Inf") {

            CHECK(is_pos_inf(model.get_best_obj()));

        }

        AND_THEN("The returned certificate should be valid") {

            const auto farkas = save_farkas_values(model);

            CHECK(farkas.status() == Infeasible);

            const double c1_val = farkas.get(c1);
            const double c2_val = farkas.get(c2);

            CHECK(1. * c1_val - 2. * c2_val <= 0. );
            CHECK(-2. * c1_val + 1. * c2_val <= 0. );
            CHECK(-1. * c1_val + -1. * c2_val <= 0. );

        }


    }

}