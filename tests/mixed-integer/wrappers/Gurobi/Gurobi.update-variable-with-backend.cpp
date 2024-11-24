//
// Created by henri on 31/01/23.
//

#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"
#include <catch2/catch_all.hpp>
#include "idol/modeling.h"

using namespace Catch::literals;
using namespace idol;

#ifdef IDOL_USE_GUROBI

SCENARIO("Gurobi: Update a variable with backend", "[unit][backend][Gurobi]") {

    GIVEN("An initially empty model with a Gurobi backend") {

        Env env;
        Model model(env);

        model.use(Gurobi());

        const auto& backend = ((const Model &) model).optimizer().as<Optimizers::Gurobi>();

        WHEN("A continuous variable (lb=0,ub=1) is added") {

            Var x(env, 0., 1., Continuous, 0., "x");
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

                model.set_var_ub(x, 0);

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

#endif
