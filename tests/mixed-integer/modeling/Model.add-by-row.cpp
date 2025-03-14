//
// Created by henri on 30/11/22.
//
#include <catch2/catch_all.hpp>
#include "idol/modeling.h"

using namespace Catch::literals;
using namespace idol;

SCENARIO("Model: Add a constraint by row", "[unit][modeling-old][Model]") {

    Env env;
    Model model(env);

    GIVEN("An initial model with some variables and no constraint") {

        auto x = Var::make_vector(env, Dim<1>(3), 0., 1., Continuous, 0.);
        model.add_vector<Var, 1>(x);

        WHEN("Adding a constraint by row (linear and quadratic)") {

            LinExpr<Var> row;
            row.set(x[0], 101);
            row.set(x[1], 102);
            row.set(x[2], 103);
            /*
            row.quadratic().set(x[0], x[0], 104);
            row.quadratic().set(x[0], x[1], 105);
            row.quadratic().set(x[0], x[2], 106);
            row.quadratic().set(x[1], x[1], 107);
            row.quadratic().set(x[1], x[2], 108);
            row.quadratic().set(x[2], x[2], 109);
            */

            Ctr ctr(env, TempCtr(std::move(row), LessOrEqual, 100));
            model.add(ctr);

            THEN("The number of constraints should be one") {
                CHECK(model.ctrs().size() == 1);
            }

            AND_THEN("The model should have the constraint") {
                CHECK(model.has(ctr));
            }

            AND_THEN("The constraint should be in the model") {
                CHECK(ctr.is_in(model));
            }

            AND_THEN("The row rhs should be added to the model's rhs") {
                CHECK(model.get_rhs_expr().get(ctr) == 100_a);
            }

            AND_THEN("The row rhs should have been added to the model's constraint's rhs") {
                CHECK(model.get_ctr_rhs(ctr) == 100_a);
            }

            AND_THEN("The row coefficients should have been added to the model's constraints' coefficients") {
                CHECK(model.get_ctr_row(ctr).get(x[0]) == 101_a);
                CHECK(model.get_ctr_row(ctr).get(x[1]) == 102_a);
                CHECK(model.get_ctr_row(ctr).get(x[2]) == 103_a);

                /*
                CHECK(model.get_ctr_row(original_ctr).quadratic().get(x[0], x[0]) == 104_a);
                CHECK(model.get_ctr_row(original_ctr).quadratic().get(x[0], x[1]) == 105_a);
                CHECK(model.get_ctr_row(original_ctr).quadratic().get(x[0], x[2]) == 106_a);
                CHECK(model.get_ctr_row(original_ctr).quadratic().get(x[1], x[1]) == 107_a);
                CHECK(model.get_ctr_row(original_ctr).quadratic().get(x[1], x[2]) == 108_a);
                CHECK(model.get_ctr_row(original_ctr).quadratic().get(x[2], x[2]) == 109_a);
                 */
            }

            AND_THEN("The row coefficients should have been added to the model's variables' coefficients") {
                CHECK(model.get_var_column(x[0]).get(ctr) == 101_a);
                //CHECK(model.get_var_column(x[0]).quadratic().get(original_ctr, x[0]) == 104_a);
                //CHECK(model.get_var_column(x[0]).quadratic().get(original_ctr, x[1]) == 105_a);
                //CHECK(model.get_var_column(x[0]).quadratic().get(original_ctr, x[2]) == 106_a);

                CHECK(model.get_var_column(x[1]).get(ctr) == 102_a);
                //CHECK(model.get_var_column(x[1]).quadratic().get(original_ctr, x[0]) == 105_a);
                //CHECK(model.get_var_column(x[1]).quadratic().get(original_ctr, x[1]) == 107_a);
                //CHECK(model.get_var_column(x[1]).quadratic().get(original_ctr, x[2]) == 108_a);

                CHECK(model.get_var_column(x[2]).get(ctr) == 103_a);
                //CHECK(model.get_var_column(x[2]).quadratic().get(original_ctr, x[0]) == 106_a);
                //CHECK(model.get_var_column(x[2]).quadratic().get(original_ctr, x[1]) == 108_a);
                //CHECK(model.get_var_column(x[2]).quadratic().get(original_ctr, x[2]) == 109_a);
            }

            AND_WHEN("The rhs of that constraint in changed int the model (nonzero)") {

                model.set_ctr_rhs(ctr, 1);

                THEN("The model's rhs should be updated") {
                    CHECK(model.get_rhs_expr().get(ctr) == 1_a);
                }

                AND_THEN("The constraint's rhs should be updated") {
                    CHECK(model.get_ctr_rhs(ctr) == 1_a);
                }

            }

            AND_WHEN("A matrix coefficient is changed in the model (nonzero)") {

                model.set_mat_coeff(ctr, x[0], 1);

                THEN("The model's constraint's row should be updated") {
                    CHECK(model.get_ctr_row(ctr).get(x[0]) == 1_a);
                }

                AND_THEN("The model's variable's column should be updated") {
                    CHECK(model.get_var_column(x[0]).get(ctr) == 1_a);
                }

            }

            AND_WHEN("An rhs coefficient is set to zero") {

                model.set_ctr_rhs(ctr, 0);

                THEN("The model's objective should be empty") {
                    SKIP(model.get_rhs_expr().empty());
                }

                AND_THEN("The model's constraint's row's rhs should be zero") {
                    CHECK(model.get_ctr_rhs(ctr) == 0_a);
                }

            }


            AND_WHEN("A matrix coefficient is set to zero") {

                model.set_mat_coeff(ctr, x[0], 0);

                THEN("The model's constraint's row should be empty") {
                    CHECK(model.get_ctr_row(ctr).get(x[0]) == 0_a);
                }

                AND_THEN("The model's variable's column's coefficient should be zero") {
                    CHECK(model.get_var_column(x[0]).get(ctr) == 0_a);
                }

            }

            AND_WHEN("The constraint is removed") {

                model.remove(ctr);

                THEN("The number of constraints should be zero") {
                    CHECK(model.ctrs().size() == 0);
                }

                AND_THEN("The model should not have the constraint") {
                    CHECK(!model.has(ctr));
                }

                AND_THEN("The constraint should not be part of the model") {
                    CHECK(!ctr.is_in(model));
                }

                AND_THEN("The model's rhs should be empty") {
                    SKIP(model.get_rhs_expr().empty());
                }

                AND_THEN("The model's columns should be empty") {
                    SKIP(model.get_var_column(x[0]).empty());
                    //CHECK(model.get_var_column(x[0]).quadratic().empty());

                    SKIP(model.get_var_column(x[1]).empty());
                    //CHECK(model.get_var_column(x[1]).quadratic().empty());

                    SKIP(model.get_var_column(x[2]).empty());
                    //CHECK(model.get_var_column(x[2]).quadratic().empty());
                }

                AND_THEN("The constraint's coefficients in the model's variables should be zero") {
                    CHECK(model.get_var_column(x[0]).get(ctr) == 0_a);
                    //CHECK(model.get_var_column(x[0]).quadratic().get(original_ctr, x[0]) == 0_a);
                    //CHECK(model.get_var_column(x[0]).quadratic().get(original_ctr, x[1]) == 0_a);
                    //CHECK(model.get_var_column(x[0]).quadratic().get(original_ctr, x[2]) == 0_a);

                    CHECK(model.get_var_column(x[1]).get(ctr) == 0_a);
                    //CHECK(model.get_var_column(x[1]).quadratic().get(original_ctr, x[0]) == 0_a);
                    //CHECK(model.get_var_column(x[1]).quadratic().get(original_ctr, x[1]) == 0_a);
                    //CHECK(model.get_var_column(x[1]).quadratic().get(original_ctr, x[2]) == 0_a);

                    CHECK(model.get_var_column(x[2]).get(ctr) == 0_a);
                    //CHECK(model.get_var_column(x[2]).quadratic().get(original_ctr, x[0]) == 0_a);
                    //CHECK(model.get_var_column(x[2]).quadratic().get(original_ctr, x[1]) == 0_a);
                    //CHECK(model.get_var_column(x[2]).quadratic().get(original_ctr, x[2]) == 0_a);
                }

            }

        }

    }

}