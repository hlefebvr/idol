//
// Created by henri on 01/02/23.
//

#include "../../../../test_utils.h"
#include "optimizers/solvers/Gurobi.h"
#include "optimizers/solvers/DefaultOptimizer.h"

#ifdef IDOL_USE_GUROBI

SCENARIO("Gurobi: Update objective with backend", "[unit][backend][Gurobi]") {

    GIVEN("A model with two variables x and y") {

        Env env;

        Var x(env, 0., 1., Continuous, "x");
        Var y(env, 0., 1., Continuous, "y");

        Model model(env);
        model.add(x);
        model.add(y);

        model.use(GurobiOptimizer());

        const auto& backend = ((const Model &) model).optimizer().as<Optimizers::Gurobi>();

        model.update();

        WHEN("Model::update_objective() is called") {

            THEN("The objective coefficient for x should be 0") {
                CHECK(backend[x].get(GRB_DoubleAttr_Obj) == 0._a);
            }

            THEN("The objective coefficient for y should be 0") {
                CHECK(backend[y].get(GRB_DoubleAttr_Obj) == 0._a);
            }

        }

        WHEN("The objective coefficient for x is changed to 1 and Model::update_objective() is called") {

            model.set(Attr::Var::Obj, x, 1);
            model.update();


            THEN("The objective coefficient for x should be 1") {
                CHECK(backend[x].get(GRB_DoubleAttr_Obj) == 1._a);
            }

            THEN("The objective coefficient for y should be 0") {
                CHECK(backend[y].get(GRB_DoubleAttr_Obj) == 0._a);
            }

        }


        WHEN("The objective is set to 1 * x and Model::update_objective() is called") {

            model.set(Attr::Obj::Expr, x);
            model.update();


            THEN("The objective coefficient for x should be 1") {
                CHECK(backend[x].get(GRB_DoubleAttr_Obj) == 1._a);
            }

            THEN("The objective coefficient for y should be 0") {
                CHECK(backend[y].get(GRB_DoubleAttr_Obj) == 0._a);
            }

            AND_WHEN("The objective is set to 1 * y and Model::update_objective() is called") {

                model.set(Attr::Obj::Expr, y);
                model.update();


                THEN("The objective coefficient for x should be 0") {
                    CHECK(backend[x].get(GRB_DoubleAttr_Obj) == 0._a);
                }

                THEN("The objective coefficient for y should be 0") {
                    CHECK(backend[y].get(GRB_DoubleAttr_Obj) == 1._a);
                }

            }

        }

    }

}

#endif
