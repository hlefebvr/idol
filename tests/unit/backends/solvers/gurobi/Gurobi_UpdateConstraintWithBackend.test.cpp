//
// Created by henri on 31/01/23.
//

#include "../../../../test_utils.h"
#include "backends/solvers/Gurobi.h"

#ifdef IDOL_USE_GUROBI

SCENARIO("Gurobi: Update a constraint with backend", "[unit][backend][Gurobi]") {

    GIVEN("An initially empty model with a Gurobi backend") {

        Env env;
        Model model(env);
        auto& backend = Idol::set_optimizer<Gurobi>(model);

        WHEN("A <=-constraint (rhs=1) is added") {

            Ctr c(env, LessOrEqual, 1, "c");
            model.add(c);

            WHEN("The constraint is removed before Model::update_objective() is called") {

                model.remove(c);

                WHEN("Model::update_objective() is called") {

                    model.update();

                    THEN("The constraint should not be part of the model") {
                        CHECK_THROWS(backend[c]);
                    }

                }

            }

            AND_WHEN("The rhs is updated to 0 before Model::update_objective() is called") {

                model.set(Attr::Ctr::Rhs, c, 0);

                WHEN("Model::update_objective() is called") {

                    model.update();

                    THEN("The variable's upper bound should be 0") {
                        CHECK(std::get<GRBConstr>(backend[c]).get(GRB_DoubleAttr_RHS) ==  0._a);
                    }

                }

            }

        }

    }

}

#endif
