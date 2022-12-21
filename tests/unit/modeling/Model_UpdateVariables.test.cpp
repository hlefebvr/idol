//
// Created by henri on 21/12/22.
//

#include "../../test_utils.h"

SCENARIO("Model: Update a variable", "[unit][modeling][Model]") {

    Model model;

    GIVEN("An initial model with no variable and no constraint") {


        WHEN("Adding a continuous variable with infinite lower and upper bound and no objective function") {

            auto x = model.add_var(-Inf, Inf, Continuous, 0., "x");

            THEN("The variabe's objective coefficient should be zero") {
                CHECK(model.get(Attr::Var::Obj, x).numerical() == 0);
                CHECK(model.get(Attr::Var::Obj, x).is_zero());
                CHECK(model.get(Attr::Var::Obj, x).is_numerical());
            }

            AND_THEN("The model's objective should not contain a non-zero coefficient x") {
                auto objective = model.get(Attr::Obj::Expr);

                CHECK(objective.linear().empty());
                CHECK(objective.quadratic().empty());
                CHECK(objective.constant().is_zero());
                CHECK(objective.is_zero());
            }

            AND_THEN("The variable's lower bound should be -Inf") {
                CHECK(model.get(Attr::Var::Lb, x) == -Inf);
                CHECK(is_neg_inf(model.get(Attr::Var::Lb, x)));
            }

            AND_THEN("The variable's upper bound should be +Inf") {
                CHECK(model.get(Attr::Var::Ub, x) == +Inf);
                CHECK(is_pos_inf(model.get(Attr::Var::Ub, x)));
            }

            AND_THEN("The variable's type should be Continuous") {
                CHECK(model.get(Attr::Var::Type, x) == Continuous);
            }

            AND_WHEN("The variable's lower bound is set to 0") {

                model.set(Attr::Var::Lb, x, 0);

                THEN("The variable's lower bound should be 0") {
                    CHECK(model.get(Attr::Var::Lb, x) == 0);
                }

            }

            AND_WHEN("The variable's upper bound is set to 0") {

                model.set(Attr::Var::Ub, x, 0);

                THEN("The variable's upper bound should be 0") {
                    CHECK(model.get(Attr::Var::Ub, x) == 0);
                }

            }

            AND_WHEN("The variable's objective coefficient is set to 1") {

                model.set(Attr::Var::Obj, x, 1);

                THEN("The variable's objective coefficient should be 1") {
                    CHECK(model.get(Attr::Var::Obj, x).numerical() == 1);
                    CHECK(model.get(Attr::Var::Obj, x).is_numerical());
                }

                THEN("The model's objective should have a coefficient for x of 1") {
                    auto objective = model.get(Attr::Obj::Expr);

                    CHECK(objective.linear().get(x).numerical() == 1);
                    CHECK(objective.linear().get(x).is_numerical());
                }

            }

        }

    }

}