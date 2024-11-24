//
// Created by henri on 30/11/22.
//
#include <catch2/catch_all.hpp>
#include "idol/modeling.h"

using namespace Catch::literals;
using namespace idol;

SCENARIO("Model: Add a variable by column", "[unit][modeling-old][Model]") {

    Env env;

    auto storage = GENERATE(Model::Storage::RowOriented, Model::Storage::ColumnOriented, Model::Storage::Both);

    Model model(env);

    GIVEN("An initial model with some constraints and no variable") {

        auto c = Ctr::make_vector(env, Dim<1>(3), LessOrEqual, 0.);
        model.add_vector<Ctr, 1>(c);

        WHEN("Adding a variable by column (linear only)") {

            LinExpr<Ctr> column;
            column.set(c[0], 101);
            column.set(c[1], 102);
            column.set(c[2], 103);

            Var var(env, 0., 1, Continuous, 100, column);
            model.add(var);

            THEN("The number of variables should be one") {
                CHECK(model.vars().size() == 1);
            }

            AND_THEN("The model should be in the model") {
                CHECK(var.is_in(model));
            }

            AND_THEN("The column objective should have been added to the model's variable objective") {
                CHECK(model.get_var_obj(var) == 100_a);
            }

            AND_THEN("The column objective should have been added to the model's objective") {
                CHECK(model.get_obj_expr().affine().linear().get(var) == 100_a);
            }

            AND_THEN("The column coefficients should have been added to the model's variable column") {
                CHECK(model.get_var_column(var).get(c[0]) == 101_a);
                CHECK(model.get_var_column(var).get(c[1]) == 102_a);
                CHECK(model.get_var_column(var).get(c[2]) == 103_a);
            }

            AND_THEN("The column coefficients should have been added to the model's constraints' rows") {
                CHECK(model.get_ctr_row(c[0]).get(var) == 101_a);
                CHECK(model.get_ctr_row(c[1]).get(var) == 102_a);
                CHECK(model.get_ctr_row(c[2]).get(var) == 103_a);
            }

            AND_WHEN("The objective coefficient of that variable is changed in the model (nonzero)") {

                model.set_var_obj(var, 1);

                THEN("The model's objective should be updated") {
                    CHECK(model.get_obj_expr().affine().linear().get(var) == 1_a);
                }

                AND_THEN("The variable's objective coefficient should be updated") {
                    CHECK(model.get_var_obj(var) == 1_a);
                }
            }

            AND_WHEN("A matrix coefficient is changed in the model (nonzero)") {

                model.set_mat_coeff(c[0], var, 1);

                THEN("The model's constraint's row should be updated") {
                    CHECK(model.get_ctr_row(c[0]).get(var) == 1_a);
                }

                AND_THEN("The model's variable's column should be updated") {
                    CHECK(model.get_var_column(var).get(c[0]) == 1_a);
                }

            }

            AND_WHEN("An objective coefficient is set to zero") {

                model.set_var_obj(var, 0);

                THEN("The model's objective should be empty") {
                    SKIP(model.get_obj_expr().affine().linear().empty());
                }

                AND_THEN("The model's variable's column's objective coefficient should be zero") {
                    CHECK(model.get_var_obj(var) == 0_a);
                }

            }

            AND_WHEN("A constraint coefficient is set to zero") {

                model.set_mat_coeff(c[0], var, 0);

                THEN("The model's constraint's row should be empty") {
                    SKIP(model.get_ctr_row(c[0]).empty());
                }

                AND_THEN("The model's variable's column's coefficient should be zero") {
                    CHECK(model.get_var_column(var).get(c[0]) == 0_a);
                }

            }

            AND_WHEN("The variable is removed") {

                model.remove(var);

                THEN("The number of variables should be zero") {
                    CHECK(model.vars().size() == 0);
                }

                AND_THEN("The model should not have the variable") {
                    CHECK(!model.has(var));
                }

                AND_THEN("The variable should not be part of the model") {
                    CHECK(!var.is_in(model));
                }

                AND_THEN("The model's objective should be empty") {
                    CHECK(model.get_obj_expr().affine().linear().empty());
                }

                AND_THEN("The model's constraints should be empty") {
                    CHECK(model.get_ctr_row(c[0]).empty());
                    CHECK(model.get_ctr_row(c[1]).empty());
                    CHECK(model.get_ctr_row(c[2]).empty());
                }

                AND_THEN("The variable's coefficients in the model's constraints should be zero") {
                    CHECK(model.get_ctr_row(c[0]).get(var) == 0_a);
                    CHECK(model.get_ctr_row(c[1]).get(var) == 0_a);
                    CHECK(model.get_ctr_row(c[2]).get(var) == 0_a);
                }

            }
        }

    }

    GIVEN("An initial model with some constraints and some variables") {

        auto c = Ctr::make_vector(env, Dim<1>(3), LessOrEqual, 0.);
        model.add_vector<Ctr, 1>(c);

        auto x = Var::make_vector(env, Dim<1>(3), 0., 1., Continuous, 0.);
        model.add_vector<Var, 1>(x);

        WHEN("Adding a variable by column (linear and quadratic)") {

            LinExpr<Ctr> column;
            column.set(c[0], 101);
            column.set(c[1], 102);
            column.set(c[2], 103);
            /*
            column.quadratic().set(c[0], x[0], 104);
            column.quadratic().set(c[0], x[1], 105);
            column.quadratic().set(c[0], x[2], 106);
            column.quadratic().set(c[1], x[0], 107);
            column.quadratic().set(c[1], x[1], 108);
            column.quadratic().set(c[1], x[2], 109);
            column.quadratic().set(c[2], x[0], 110);
            column.quadratic().set(c[2], x[1], 111);
            column.quadratic().set(c[2], x[2], 112);
            */

            Var var(env, 0., 1., Continuous, 100, column);
            model.add(var);

            THEN("The number of variables should increase by one") {
                CHECK(model.vars().size() == 4);
            }

            // OBJECTIVE check model's objective and variable's objective

            AND_THEN("The column coefficients should have been added to the model's variable column") {
                CHECK(model.get_var_column(var).get(c[0]) == 101_a);
                CHECK(model.get_var_column(var).get(c[1]) == 102_a);
                CHECK(model.get_var_column(var).get(c[2]) == 103_a);

                /*
                CHECK(model.get_var_column(var).quadratic().get(c[0], x[0]) == 104_a);
                CHECK(model.get_var_column(var).quadratic().get(c[0], x[1]) == 105_a);
                CHECK(model.get_var_column(var).quadratic().get(c[0], x[2]) == 106_a);

                CHECK(model.get_var_column(var).quadratic().get(c[1], x[0]) == 107_a);
                CHECK(model.get_var_column(var).quadratic().get(c[1], x[1]) == 108_a);
                CHECK(model.get_var_column(var).quadratic().get(c[1], x[2]) == 109_a);

                CHECK(model.get_var_column(var).quadratic().get(c[2], x[0]) == 110_a);
                CHECK(model.get_var_column(var).quadratic().get(c[2], x[1]) == 111_a);
                CHECK(model.get_var_column(var).quadratic().get(c[2], x[2]) == 112_a);
                 */
            }

            AND_THEN("The column coefficients should have been added to the model's constraints' rows") {
                CHECK(model.get_ctr_row(c[0]).get(var) == 101_a);
                CHECK(model.get_ctr_row(c[1]).get(var) == 102_a);
                CHECK(model.get_ctr_row(c[2]).get(var) == 103_a);

                /*
                CHECK(model.get_ctr_row(c[0]).quadratic().get(var, x[0]) == 104_a);
                CHECK(model.get_ctr_row(c[0]).quadratic().get(var, x[1]) == 105_a);
                CHECK(model.get_ctr_row(c[0]).quadratic().get(var, x[2]) == 106_a);

                CHECK(model.get_ctr_row(c[1]).quadratic().get(var, x[0]) == 107_a);
                CHECK(model.get_ctr_row(c[1]).quadratic().get(var, x[1]) == 108_a);
                CHECK(model.get_ctr_row(c[1]).quadratic().get(var, x[2]) == 109_a);

                CHECK(model.get_ctr_row(c[2]).quadratic().get(var, x[0]) == 110_a);
                CHECK(model.get_ctr_row(c[2]).quadratic().get(var, x[1]) == 111_a);
                CHECK(model.get_ctr_row(c[2]).quadratic().get(var, x[2]) == 112_a);
                 */
            }

            AND_WHEN("The variable is removed") {

                model.remove(var);

                THEN("The number of variables should be decreased by 1") {
                    CHECK(model.vars().size() == 3);
                }

                AND_THEN("The model should not have the variable") {
                    CHECK(!model.has(var));
                }

                AND_THEN("The model's objective should be empty") {
                    SKIP(model.get_obj_expr().affine().linear().empty());
                    //CHECK(model.get_obj_expr().quadratic().empty());
                }

                AND_THEN("The model's constraints should be empty") {
                    SKIP(model.get_ctr_row(c[0]).empty());
                    SKIP(model.get_ctr_row(c[1]).empty());
                    SKIP(model.get_ctr_row(c[2]).empty());

                    /*
                    CHECK(model.get_ctr_row(c[0]).quadratic().empty());
                    CHECK(model.get_ctr_row(c[1]).quadratic().empty());
                    CHECK(model.get_ctr_row(c[2]).quadratic().empty());
                     */
                }

                AND_THEN("The variable's coefficients in the model's constraints should be zero") {
                    CHECK(model.get_ctr_row(c[0]).get(var) == 0_a);
                    CHECK(model.get_ctr_row(c[1]).get(var) == 0_a);
                    CHECK(model.get_ctr_row(c[2]).get(var) == 0_a);

                    /*
                    CHECK(model.get_ctr_row(c[0]).quadratic().get(var, x[0]) == 0_a);
                    CHECK(model.get_ctr_row(c[1]).quadratic().get(var, x[1]) == 0_a);
                    CHECK(model.get_ctr_row(c[2]).quadratic().get(var, x[2]) == 0_a);
                     */
                }

            }

        }

    }

}
