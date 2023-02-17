//
// Created by henri on 31/01/23.
//

#include "../../../../test_utils.h"
#include "backends/solvers/Gurobi.h"

SCENARIO("Gurobi: Update a variable with backend", "[unit][backend][Gurobi]") {

    GIVEN("An initially empty model with a Gurobi backend") {

        Env env;
        Model model(env);
        auto& backend = Idol::set_optimizer<Gurobi>(model);

        WHEN("A continuous variable (lb=0,ub=1) is added") {

            Var x(env, 0., 1., Continuous, "x");
            model.add(x);

            WHEN("The variable is removed before Model::update_objective() is called") {

                model.remove(x);

                WHEN("Model::update_objective() is called") {

                    model.update();

                    THEN("The variable should not be part of the model") {
                        CHECK_THROWS(backend[x]);
                    }

                }

            }

            AND_WHEN("The upper bound is updated to 0 before Model::update_objective() is called") {

                model.set(Attr::Var::Ub, x, 0);

                WHEN("Model::update_objective() is called") {

                    model.update();

                    THEN("The variable's upper bound should be 0") {
                        CHECK(backend[x].get(GRB_DoubleAttr_UB) ==  0._a);
                    }

                }

            }

        }

    }

}