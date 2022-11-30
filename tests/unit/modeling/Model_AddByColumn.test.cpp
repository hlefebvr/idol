//
// Created by henri on 30/11/22.
//

#include "../../test_utils.h"

SCENARIO("Model: Add a variable by column", "[unit][modeling][Model]") {

    Model model;

    GIVEN("An initial model with some constraints and no variable") {

        auto c = model.add_ctrs(Dim<1>(3), LessOrEqual, 0.);

        WHEN("Adding a variable by column (linear only)") {

            Column column;
            column.set_obj(100);
            column.linear().set(c[0], 101);
            column.linear().set(c[1], 102);
            column.linear().set(c[2], 103);

            auto var = model.add_var(0., 1, Continuous, column);

            THEN("The number of variables should be one") {
                CHECK(model.vars().size() == 1);
            }

            AND_THEN("The column objective should have been added to the model's variable objective") {
                CHECK(model.get(Attr::Var::Column, var).obj().numerical() == 100_a);
            }

            AND_THEN("The column objective should have been added to the model's objective") {
                CHECK(model.get(Attr::Obj::Expr).linear().get(var).numerical() == 100_a);
            }

            AND_THEN("The column coefficients should have been added to the model's variable column") {
                CHECK(model.get(Attr::Var::Column, var).linear().get(c[0]).numerical() == 101_a);
                CHECK(model.get(Attr::Var::Column, var).linear().get(c[1]).numerical() == 102_a);
                CHECK(model.get(Attr::Var::Column, var).linear().get(c[2]).numerical() == 103_a);
            }

            AND_THEN("The column coefficients should have been added to the model's constraints' rows") {
                CHECK(model.get(Attr::Ctr::Row, c[0]).linear().get(var).numerical() == 101_a);
                CHECK(model.get(Attr::Ctr::Row, c[1]).linear().get(var).numerical() == 102_a);
                CHECK(model.get(Attr::Ctr::Row, c[2]).linear().get(var).numerical() == 103_a);
            }

            AND_WHEN("The objective coefficient of that variable is changed in the model (nonzero)") {

                model.set(Attr::Var::Obj, var, 1);

                THEN("The model's objective should be updated") {
                    CHECK(model.get(Attr::Obj::Expr).linear().get(var).numerical() == 1_a);
                }

                AND_THEN("The variable's objective coefficient should be updated") {
                    CHECK(model.get(Attr::Var::Obj, var).numerical() == 1_a);
                }
            }

            AND_WHEN("A matrix coefficient is changed in the model (nonzero)") {

                model.set(Attr::Matrix::Coeff, c[0], var, 1);

                THEN("The model's constraint's row should be updated") {
                    CHECK(model.get(Attr::Ctr::Row, c[0]).linear().get(var).numerical() == 1_a);
                }

                AND_THEN("The model's variable's column should be updated") {
                    CHECK(model.get(Attr::Var::Column, var).linear().get(c[0]).numerical() == 1_a);
                }

            }

            AND_WHEN("An objective coefficient is set to zero") {

                model.set(Attr::Var::Obj, var, 0);

                THEN("The model's objective should be empty") {
                    CHECK(model.get(Attr::Obj::Expr).linear().empty());
                }

                AND_THEN("The model's variable's column's objective coefficient should be zero") {
                    CHECK(model.get(Attr::Var::Obj, var).numerical() == 0_a);
                }

            }

            AND_WHEN("A constraint coefficient is set to zero") {

                model.set(Attr::Matrix::Coeff, c[0], var, 0);

                THEN("The model's constraint's row should be empty") {
                    CHECK(model.get(Attr::Ctr::Row, c[0]).linear().empty());
                }

                AND_THEN("The model's variable's column's coefficient should be zero") {
                    CHECK(model.get(Attr::Var::Column, var).linear().get(c[0]).numerical() == 0_a);
                }

            }

            AND_WHEN("The variable is removed") {

                model.remove(var);

                THEN("The number of variables should be zero") {
                    CHECK(model.vars().size() == 0);
                }

                AND_THEN("The variable should have status 0 in the model") {
                    CHECK(model.get(Attr::Var::Status, var) == 0);
                }

                AND_THEN("The model's objective should be empty") {
                    CHECK(model.get(Attr::Obj::Expr).linear().empty());
                }

                AND_THEN("The model's constraints should be empty") {
                    CHECK(model.get(Attr::Ctr::Row, c[0]).linear().empty());
                    CHECK(model.get(Attr::Ctr::Row, c[1]).linear().empty());
                    CHECK(model.get(Attr::Ctr::Row, c[2]).linear().empty());
                }

                AND_THEN("The variable's coefficients in the model's constraints should be zero") {
                    CHECK(model.get(Attr::Ctr::Row, c[0]).linear().get(var).numerical() == 0_a);
                    CHECK(model.get(Attr::Ctr::Row, c[1]).linear().get(var).numerical() == 0_a);
                    CHECK(model.get(Attr::Ctr::Row, c[2]).linear().get(var).numerical() == 0_a);
                }

            }
        }

    }

    GIVEN("An initial model with some constraints and some variables") {

        auto c = model.add_ctrs(Dim<1>(3), LessOrEqual, 0.);
        auto x = model.add_vars(Dim<1>(3), 0., 1., Continuous, 0);

        WHEN("Adding a variable by column (linear and quadratic)") {

            Column column;
            column.set_obj(100);
            column.linear().set(c[0], 101);
            column.linear().set(c[1], 102);
            column.linear().set(c[2], 103);
            column.quadratic().set(c[0], x[0], 104);
            column.quadratic().set(c[0], x[1], 105);
            column.quadratic().set(c[0], x[2], 106);
            column.quadratic().set(c[1], x[0], 107);
            column.quadratic().set(c[1], x[1], 108);
            column.quadratic().set(c[1], x[2], 109);
            column.quadratic().set(c[2], x[0], 110);
            column.quadratic().set(c[2], x[1], 111);
            column.quadratic().set(c[2], x[2], 112);

            auto var = model.add_var(0., 1, Continuous, column);

            THEN("The number of variables should increase by one") {
                CHECK(model.vars().size() == 4);
            }

            // OBJECTIVE check model's objective and variable's objective

            AND_THEN("The column coefficients should have been added to the model's variable column") {
                CHECK(model.get(Attr::Var::Column, var).linear().get(c[0]).numerical() == 101_a);
                CHECK(model.get(Attr::Var::Column, var).linear().get(c[1]).numerical() == 102_a);
                CHECK(model.get(Attr::Var::Column, var).linear().get(c[2]).numerical() == 103_a);

                CHECK(model.get(Attr::Var::Column, var).quadratic().get(c[0], x[0]).numerical() == 104_a);
                CHECK(model.get(Attr::Var::Column, var).quadratic().get(c[0], x[1]).numerical() == 105_a);
                CHECK(model.get(Attr::Var::Column, var).quadratic().get(c[0], x[2]).numerical() == 106_a);

                CHECK(model.get(Attr::Var::Column, var).quadratic().get(c[1], x[0]).numerical() == 107_a);
                CHECK(model.get(Attr::Var::Column, var).quadratic().get(c[1], x[1]).numerical() == 108_a);
                CHECK(model.get(Attr::Var::Column, var).quadratic().get(c[1], x[2]).numerical() == 109_a);

                CHECK(model.get(Attr::Var::Column, var).quadratic().get(c[2], x[0]).numerical() == 110_a);
                CHECK(model.get(Attr::Var::Column, var).quadratic().get(c[2], x[1]).numerical() == 111_a);
                CHECK(model.get(Attr::Var::Column, var).quadratic().get(c[2], x[2]).numerical() == 112_a);
            }

            AND_THEN("The column coefficients should have been added to the model's constraints' rows") {
                CHECK(model.get(Attr::Ctr::Row, c[0]).linear().get(var).numerical() == 101_a);
                CHECK(model.get(Attr::Ctr::Row, c[1]).linear().get(var).numerical() == 102_a);
                CHECK(model.get(Attr::Ctr::Row, c[2]).linear().get(var).numerical() == 103_a);

                CHECK(model.get(Attr::Ctr::Row, c[0]).quadratic().get(var, x[0]).numerical() == 104_a);
                CHECK(model.get(Attr::Ctr::Row, c[0]).quadratic().get(var, x[1]).numerical() == 105_a);
                CHECK(model.get(Attr::Ctr::Row, c[0]).quadratic().get(var, x[2]).numerical() == 106_a);

                CHECK(model.get(Attr::Ctr::Row, c[1]).quadratic().get(var, x[0]).numerical() == 107_a);
                CHECK(model.get(Attr::Ctr::Row, c[1]).quadratic().get(var, x[1]).numerical() == 108_a);
                CHECK(model.get(Attr::Ctr::Row, c[1]).quadratic().get(var, x[2]).numerical() == 109_a);

                CHECK(model.get(Attr::Ctr::Row, c[2]).quadratic().get(var, x[0]).numerical() == 110_a);
                CHECK(model.get(Attr::Ctr::Row, c[2]).quadratic().get(var, x[1]).numerical() == 111_a);
                CHECK(model.get(Attr::Ctr::Row, c[2]).quadratic().get(var, x[2]).numerical() == 112_a);
            }

            AND_WHEN("The variable is removed") {

                model.remove(var);

                THEN("The number of variables should be decreased by 1") {
                    CHECK(model.vars().size() == 3);
                }

                AND_THEN("The variable should have status 0 in the model") {
                    CHECK(model.get(Attr::Var::Status, var) == 0);
                }

                AND_THEN("The model's objective should be empty") {
                    CHECK(model.get(Attr::Obj::Expr).linear().empty());
                    CHECK(model.get(Attr::Obj::Expr).quadratic().empty());
                }

                AND_THEN("The model's constraints should be empty") {
                    CHECK(model.get(Attr::Ctr::Row, c[0]).linear().empty());
                    CHECK(model.get(Attr::Ctr::Row, c[1]).linear().empty());
                    CHECK(model.get(Attr::Ctr::Row, c[2]).linear().empty());

                    CHECK(model.get(Attr::Ctr::Row, c[0]).quadratic().empty());
                    CHECK(model.get(Attr::Ctr::Row, c[1]).quadratic().empty());
                    CHECK(model.get(Attr::Ctr::Row, c[2]).quadratic().empty());
                }

                AND_THEN("The variable's coefficients in the model's constraints should be zero") {
                    CHECK(model.get(Attr::Ctr::Row, c[0]).linear().get(var).numerical() == 0_a);
                    CHECK(model.get(Attr::Ctr::Row, c[1]).linear().get(var).numerical() == 0_a);
                    CHECK(model.get(Attr::Ctr::Row, c[2]).linear().get(var).numerical() == 0_a);

                    CHECK(model.get(Attr::Ctr::Row, c[0]).quadratic().get(var, x[0]).numerical() == 0_a);
                    CHECK(model.get(Attr::Ctr::Row, c[1]).quadratic().get(var, x[1]).numerical() == 0_a);
                    CHECK(model.get(Attr::Ctr::Row, c[2]).quadratic().get(var, x[2]).numerical() == 0_a);
                }

            }

        }

    }

}
