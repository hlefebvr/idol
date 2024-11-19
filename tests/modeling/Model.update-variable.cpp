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

            Var x(env, -Inf, Inf, Continuous,  0.,"x");
            model.add(x);

            THEN("The variabe's objective coefficient should be zero") {
                CHECK(std::abs(model.get_var_obj(x)) <= Tolerance::Sparsity);
            }

            AND_THEN("The model's objective should not contain a non-zero coefficient x") {
                auto objective = model.get_obj_expr();

                SKIP(objective.affine().linear().empty());
                //CHECK(objective.quadratic().empty());
                CHECK(objective.affine().constant() == 0_a);
                CHECK(is_zero(objective, Tolerance::Sparsity));
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
                    CHECK(model.get_var_lb(x) == 0_a);
                }

            }

            AND_WHEN("The variable's upper bound is set to 0") {

                model.set_var_ub(x, 0);

                THEN("The variable's upper bound should be 0") {
                    CHECK(model.get_var_ub(x) == 0_a);
                }

            }

            AND_WHEN("The variable's objective coefficient is set to 1") {

                model.set_var_obj(x, 1);

                THEN("The variable's objective coefficient should be 1") {
                    CHECK(model.get_var_obj(x) == 1_a);
                }

                THEN("The model's objective should have a coefficient for x of 1") {
                    const auto& objective = model.get_obj_expr();

                    CHECK(objective.affine().linear().get(x) == 1_a);
                }

            }

        }

    }

}