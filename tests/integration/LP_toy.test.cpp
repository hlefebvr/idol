//
// Created by henri on 06/02/23.
//

#include "../test_utils.h"

TEST_CASE("LP solvers: solve toy example", "[integration][backend][solver]") {

    Env env;

    WHEN("A bounded and feasible LP is solved") {

        Var x(env, 0, Inf, Continuous, "x");
        Var y(env, 0, Inf, Continuous, "y");

        Ctr c1(env, 120 * x + 210 * y <= 15000);
        Ctr c2(env, 110 * x +  30 * y <=  4000);
        Ctr c3(env,       x +       y <=    75);

        Model model(env);
        model.add_many(x, y, c1, c2, c3);
        model.set(Attr::Obj::Expr, -143 * x - 60 * y);
        model.set_backend<Gurobi>();

        model.optimize();

        THEN("The solution status should be Optimal") {

            CHECK(model.get(Attr::Solution::Status) == Optimal);

        }

        AND_THEN("The objective value should be correct") {

            CHECK(model.get(Attr::Solution::ObjVal) == -6315.625_a);

        }

        AND_THEN("The primal values should be correct") {

            const auto primal_solution = save(model, Attr::Solution::Primal);

            CHECK(primal_solution.status() == Optimal);
            CHECK(primal_solution.get(x) == 21.875_a);
            CHECK(primal_solution.get(y) == 53.125_a);

        }

        AND_THEN("The dual values should be correct") {

            const auto dual_solution = save(model, Attr::Solution::Dual);

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
        model.add_many(x, y, c1, c2, c3);
        model.set(Attr::Obj::Expr, -3 * x - 2 * y);
        model.set_backend<Gurobi>();

        model.set(Param::Algorithm::InfeasibleOrUnboundedInfo, true);

        model.optimize();

        THEN("The solution status should be Unbounded") {

            CHECK(model.get(Attr::Solution::Status) == Unbounded);

        }

        AND_THEN("The objective value should be -Inf") {

            CHECK(is_neg_inf(model.get(Attr::Solution::ObjVal)));

        }

        AND_THEN("The unbounded ray should be valid") {

            const auto ray = save(model, Attr::Solution::Ray);
            
            const double x_val = ray.get(x);
            const double y_val = ray.get(y);
            
            CHECK(-3. * x_val -2. * y_val <= 0._a);
            CHECK(-2. * x_val + y_val <= 0._a);
            CHECK(x_val + y_val >= 0_a);

        }

    }

}