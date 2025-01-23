//
// Created by henri on 01/02/23.
//

#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"
#include <catch2/catch_all.hpp>
#include "idol/modeling.h"

using namespace Catch::literals;
using namespace idol;

#ifdef IDOL_USE_GUROBI

SCENARIO("Gurobi: Update objective with backend", "[unit][backend][Gurobi]") {

    GIVEN("A model with two variables x and y") {

        Env env;

        Var x(env, 0., 1., Continuous, 0., "x");
        Var y(env, 0., 1., Continuous, 0., "y");

        Model model(env);
        model.add(x);
        model.add(y);

        model.use(Gurobi());

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

            model.set_var_obj(x, 1);
            model.update();


            THEN("The objective coefficient for x should be 1") {
                CHECK(backend[x].get(GRB_DoubleAttr_Obj) == 1._a);
            }

            THEN("The objective coefficient for y should be 0") {
                CHECK(backend[y].get(GRB_DoubleAttr_Obj) == 0._a);
            }

        }


        WHEN("The objective is set to 1 * x and Model::update_objective() is called") {

            model.set_obj_expr(x);
            model.update();


            THEN("The objective coefficient for x should be 1") {
                CHECK(backend[x].get(GRB_DoubleAttr_Obj) == 1._a);
            }

            THEN("The objective coefficient for y should be 0") {
                CHECK(backend[y].get(GRB_DoubleAttr_Obj) == 0._a);
            }

            AND_WHEN("The objective is set to 1 * y and Model::update_objective() is called") {

                model.set_obj_expr(y);
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
