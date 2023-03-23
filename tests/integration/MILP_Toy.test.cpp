//
// Created by henri on 06/02/23.
//

#include "../test_utils.h"
#include "backends/solvers/DefaultOptimizer.h"
#include "backends/branch-and-bound-v2/nodes/NodeInfo.h"
#include "backends/branch-and-bound-v2/BranchAndBoundOptimizer.h"
#include "backends/branch-and-bound-v2/branching-rules/factories/MostInfeasible.h"
#include "backends/branch-and-bound-v2/relaxations/impls/ContinuousRelaxation.h"
#include "backends/branch-and-bound-v2/node-selection-rules/factories/BestBound.h"

TEMPLATE_LIST_TEST_CASE("MILP solvers: solve toy example",
                        "[integration][backend][solver]",
                        milp_solvers) {
    
    auto solver = GENERATE(
                std::shared_ptr<OptimizerFactory>(new DefaultOptimizer<TestType>),
                std::shared_ptr<OptimizerFactory>(new BranchAndBoundOptimizer<NodeInfo>(
                            DefaultOptimizer<TestType>(),
                            ContinuousRelaxation(),
                            MostInfeasible(),
                            BestBound()
                        ))
            );

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
            model.add_many(x, y, z, c1, c2);
            model.set(Attr::Obj::Expr, objective);

            model.use(*solver);

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

        AND_WHEN("The root not is not integer feasible") {


            Var x(env, 0., 1., Binary, "x");
            Var y(env, 0., 1., Binary, "y");
            Var z(env, 0., 1., Binary, "z");
            Ctr c1(env, x + 2 * y + 2.5 * z <= 4);
            Ctr c2(env, x + y >= 1);
            auto objective = -x - y - 2 * z;

            Model model(env);
            model.add_many(x, y, z, c1, c2);
            model.set(Attr::Obj::Expr, objective);

            model.use(*solver);

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

    AND_WHEN("An infeasible problem is solved") {

        WHEN("The root node is infeasible") {


            Var x(env, 0., 1., Binary, "x");
            Ctr c1(env, x >= 1);
            Ctr c2(env, x <= 0);

            Model model(env);
            model.add_many(x, c1, c2);

            model.use(*solver);

            model.optimize();

            THEN("The solution status should be Infeasible") {

                CHECK(model.get(Attr::Solution::Status) == Infeasible);

            }

            THEN("The objective value should be +inf") {

                CHECK(is_pos_inf(model.get(Attr::Solution::ObjVal)));

            }

        }

        AND_WHEN("The root node is feasible") {

            Var x(env, 0., 1., Binary, "x");
            Ctr c1(env, x >= .1);
            Ctr c2(env, x <= .9);

            Model model(env);
            model.add_many(x, c1, c2);

            model.use(*solver);

            model.optimize();

            THEN("The solution status should be Infeasible") {

                CHECK(model.get(Attr::Solution::Status) == Infeasible);

            }

            THEN("The objective value should be +inf") {

                CHECK(is_pos_inf(model.get(Attr::Solution::ObjVal)));

            }

        }

    }

    AND_WHEN("An unbounded milp is solved") {

        Var x(env, -Inf, Inf, Integer, "x");
        Model model(env);
        model.add(x);
        model.set(Attr::Obj::Expr, -x);

        model.use(*solver);

        model.optimize();

        THEN("The solution status should be Unbounded") {

            CHECK(model.get(Attr::Solution::Status) == Unbounded);

        }

        THEN("The objective value should be -inf") {

            CHECK(is_neg_inf(model.get(Attr::Solution::ObjVal)));

        }

    }

}