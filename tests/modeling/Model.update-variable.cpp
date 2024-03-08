//
// Created by henri on 21/12/22.
//

#include <catch2/catch_all.hpp>
#include <idol/modeling.h>

using namespace Catch::literals;
using namespace idol;

SCENARIO("Model: Update a variable", "[unit][modeling-old][Model]") {

    Env env;

    Model model(env);

    GIVEN("An initial model with no variable and no constraint") {


        WHEN("Adding a continuous variable with infinite lower and upper bound and no objective function") {

            Var x(env, -Inf, Inf, Continuous, "x");
            model.add(x);

            THEN("The variabe's objective coefficient should be zero") {
                CHECK(model.get_var_column(x).obj().numerical() == 0);
                CHECK(model.get_var_column(x).obj().is_zero());
                CHECK(model.get_var_column(x).obj().is_numerical());
            }

            AND_THEN("The model's objective should not contain a non-zero coefficient x") {
                auto objective = model.get_obj_expr();

                CHECK(objective.linear().empty());
                CHECK(objective.quadratic().empty());
                CHECK(objective.constant().is_zero());
                CHECK(objective.is_zero());
            }

            AND_THEN("The variable's lower bound should be -Inf") {
                CHECK(model.get_var_lb(x) == -Inf);
                CHECK(is_neg_inf(model.get_var_lb(x)));
            }

            AND_THEN("The variable's upper bound should be +Inf") {
                CHECK(model.get_var_ub(x) == +Inf);
                CHECK(is_pos_inf(model.get_var_ub(x)));
            }

            AND_THEN("The variable's type should be Continuous") {
                CHECK(model.get_var_type(x) == Continuous);
            }

            AND_WHEN("The variable's lower bound is set to 0") {

                model.set_var_lb(x, 0);

                THEN("The variable's lower bound should be 0") {
                    CHECK(model.get_var_lb(x) == 0);
                }

            }

            AND_WHEN("The variable's upper bound is set to 0") {

                model.set_var_ub(x, 0);

                THEN("The variable's upper bound should be 0") {
                    CHECK(model.get_var_ub(x) == 0);
                }

            }

            AND_WHEN("The variable's objective coefficient is set to 1") {

                model.set_var_obj(x, 1);

                THEN("The variable's objective coefficient should be 1") {
                    CHECK(model.get_var_column(x).obj().numerical() == 1);
                    CHECK(model.get_var_column(x).obj().is_numerical());
                }

                THEN("The model's objective should have a coefficient for x of 1") {
                    auto objective = model.get_obj_expr();

                    CHECK(objective.linear().get(x).numerical() == 1);
                    CHECK(objective.linear().get(x).is_numerical());
                }

            }

        }

    }

}