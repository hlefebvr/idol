//
// Created by henri on 06/02/23.
//

#include "../test_utils.h"

TEST_CASE("MILP solvers: solve toy example", "[integration][backend][solver]") {

    Env env;

    WHEN("A bounded and feasible MILP is solved") { // Example taken from http://lpsolve.sourceforge.net/5.5/formulate.htm#Construct%20the%20model%20from%20a%20Programming%20Language

        Var x(env, 0., 1., Binary, "x");
        Var y(env, 0., 1., Binary, "y");
        Var z(env, 0., 1., Binary, "z");
        Ctr c1(env, x + 2 * y + 3 * z <= 4);
        Ctr c2(env, x + y >= 1);
        auto objective = - x - y - 2 * z;

        Model model(env);
        model.add_many(x, y, z, c1, c2);
        model.set(Attr::Obj::Expr, objective);
        model.set_backend<Gurobi>();

        model.optimize();

        THEN("The solution status should be Optimal") {
            CHECK(model.get(Attr::Solution::Status) == Optimal);
        }

        AND_THEN("The objective value should be correct") {
            CHECK(model.get(Attr::Solution::ObjVal) == -3_a);
        }

        AND_THEN("The returned solution values should be correct") {

            const auto primal_solution = save(model, Attr::Solution::Primal);

            CHECK(primal_solution.get(x) == 1._a);
            CHECK(primal_solution.get(y) == 0._a);
            CHECK(primal_solution.get(z) == 1._a);

        }

    }

}