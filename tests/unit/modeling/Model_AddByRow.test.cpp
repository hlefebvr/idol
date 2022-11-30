//
// Created by henri on 30/11/22.
//

#include "../../test_utils.h"

SCENARIO("Model: Add a constraint by row", "[unit][modeling][Model]") {

    Model model;

    GIVEN("An initial model with some variables and no constraint") {

        auto x = model.add_vars(Dim<1>(3), 0., 1., Continuous, 0.);

        WHEN("Adding a constraint by row (linear and quadratic)") {

            Row row;
            row.set_rhs(100);
            row.linear().set(x[0], 101);
            row.linear().set(x[1], 102);
            row.linear().set(x[2], 103);
            row.quadratic().set(x[0], x[0], 104);
            row.quadratic().set(x[0], x[1], 105);
            row.quadratic().set(x[0], x[2], 106);
            row.quadratic().set(x[1], x[1], 107);
            row.quadratic().set(x[1], x[2], 108);
            row.quadratic().set(x[2], x[2], 109);

            auto ctr = model.add_ctr(TempCtr(std::move(row), LessOrEqual));

            THEN("The number of constraints should be one") {
                CHECK(model.ctrs().size() == 1);
            }

            AND_THEN("The row rhs should be added to the model's rhs") {
                CHECK(model.get(Attr::Rhs::Expr).get(ctr).numerical() == 100_a);
            }

            AND_THEN("The row rhs should have been added to the model's constraint's rhs") {
                CHECK(model.get(Attr::Ctr::Rhs, ctr).numerical() == 100_a);
            }

            AND_THEN("The row coefficients should have been added to the model's constraints' coefficients") {
                CHECK(model.get(Attr::Ctr::Row, ctr).linear().get(x[0]).numerical() == 101_a);
                CHECK(model.get(Attr::Ctr::Row, ctr).linear().get(x[1]).numerical() == 102_a);
                CHECK(model.get(Attr::Ctr::Row, ctr).linear().get(x[2]).numerical() == 103_a);

                CHECK(model.get(Attr::Ctr::Row, ctr).quadratic().get(x[0], x[0]).numerical() == 104_a);
                CHECK(model.get(Attr::Ctr::Row, ctr).quadratic().get(x[0], x[1]).numerical() == 105_a);
                CHECK(model.get(Attr::Ctr::Row, ctr).quadratic().get(x[0], x[2]).numerical() == 106_a);
                CHECK(model.get(Attr::Ctr::Row, ctr).quadratic().get(x[1], x[1]).numerical() == 107_a);
                CHECK(model.get(Attr::Ctr::Row, ctr).quadratic().get(x[1], x[2]).numerical() == 108_a);
                CHECK(model.get(Attr::Ctr::Row, ctr).quadratic().get(x[2], x[2]).numerical() == 109_a);
            }

            AND_THEN("The row coefficients should have been added to the model's variables' coefficients") {
                CHECK(model.get(Attr::Var::Column, x[0]).linear().get(ctr).numerical() == 101_a);
                CHECK(model.get(Attr::Var::Column, x[0]).quadratic().get(ctr, x[0]).numerical() == 104_a);
                CHECK(model.get(Attr::Var::Column, x[0]).quadratic().get(ctr, x[1]).numerical() == 105_a);
                CHECK(model.get(Attr::Var::Column, x[0]).quadratic().get(ctr, x[2]).numerical() == 106_a);

                CHECK(model.get(Attr::Var::Column, x[1]).linear().get(ctr).numerical() == 102_a);
                CHECK(model.get(Attr::Var::Column, x[1]).quadratic().get(ctr, x[0]).numerical() == 105_a);
                CHECK(model.get(Attr::Var::Column, x[1]).quadratic().get(ctr, x[1]).numerical() == 107_a);
                CHECK(model.get(Attr::Var::Column, x[1]).quadratic().get(ctr, x[2]).numerical() == 108_a);

                CHECK(model.get(Attr::Var::Column, x[2]).linear().get(ctr).numerical() == 103_a);
                CHECK(model.get(Attr::Var::Column, x[2]).quadratic().get(ctr, x[0]).numerical() == 106_a);
                CHECK(model.get(Attr::Var::Column, x[2]).quadratic().get(ctr, x[1]).numerical() == 108_a);
                CHECK(model.get(Attr::Var::Column, x[2]).quadratic().get(ctr, x[2]).numerical() == 109_a);
            }

            AND_WHEN("The rhs of that constraint in changed int the model (nonzero)") {

                model.set(Attr::Ctr::Rhs, ctr, 1);

                THEN("The model's rhs should be updated") {
                    CHECK(model.get(Attr::Rhs::Expr).get(ctr).numerical() == 1_a);
                }

                AND_THEN("The constraint's rhs should be updated") {
                    CHECK(model.get(Attr::Ctr::Rhs, ctr).numerical() == 1_a);
                }

            }

            AND_WHEN("A matrix coefficient is changed in the model (nonzero)") {

                model.set(Attr::Matrix::Coeff, ctr, x[0], 1);

                THEN("The model's constraint's row should be updated") {
                    CHECK(model.get(Attr::Ctr::Row, ctr).linear().get(x[0]).numerical() == 1_a);
                }

                AND_THEN("The model's variable's column should be updated") {
                    CHECK(model.get(Attr::Var::Column, x[0]).linear().get(ctr).numerical() == 1_a);
                }

            }

            AND_WHEN("An rhs coefficient is set to zero") {

                model.set(Attr::Ctr::Rhs, ctr, 0);

                THEN("The model's objective should be empty") {
                    CHECK(model.get(Attr::Rhs::Expr).empty());
                }

                AND_THEN("The model's constraint's row's rhs should be zero") {
                    CHECK(model.get(Attr::Ctr::Rhs, ctr).numerical() == 0_a);
                }

            }


            AND_WHEN("A matrix coefficient is set to zero") {

                model.set(Attr::Matrix::Coeff, ctr, x[0], 0);

                THEN("The model's constraint's row should be empty") {
                    CHECK(model.get(Attr::Ctr::Row, ctr).linear().get(x[0]).numerical() == 0_a);
                }

                AND_THEN("The model's variable's column's coefficient should be zero") {
                    CHECK(model.get(Attr::Var::Column, x[0]).linear().get(ctr).numerical() == 0_a);
                }

            }

            AND_WHEN("The constraint is removed") {

                model.remove(ctr);

                THEN("The number of constraints should be zero") {
                    CHECK(model.ctrs().size() == 0);
                }

                AND_THEN("The constraint should have status 0 in the model") {
                    CHECK(model.get(Attr::Ctr::Status, ctr) == 0);
                }

                AND_THEN("The model's rhs should be empty") {
                    CHECK(model.get(Attr::Rhs::Expr).empty());
                }

                AND_THEN("The model's columns should be empty") {
                    CHECK(model.get(Attr::Var::Column, x[0]).linear().empty());
                    CHECK(model.get(Attr::Var::Column, x[0]).quadratic().empty());

                    CHECK(model.get(Attr::Var::Column, x[1]).linear().empty());
                    CHECK(model.get(Attr::Var::Column, x[1]).quadratic().empty());

                    CHECK(model.get(Attr::Var::Column, x[2]).linear().empty());
                    CHECK(model.get(Attr::Var::Column, x[2]).quadratic().empty());
                }

                AND_THEN("The constraint's coefficients in the model's variables should be zero") {
                    CHECK(model.get(Attr::Var::Column, x[0]).linear().get(ctr).numerical() == 0_a);
                    CHECK(model.get(Attr::Var::Column, x[0]).quadratic().get(ctr, x[0]).numerical() == 0_a);
                    CHECK(model.get(Attr::Var::Column, x[0]).quadratic().get(ctr, x[1]).numerical() == 0_a);
                    CHECK(model.get(Attr::Var::Column, x[0]).quadratic().get(ctr, x[2]).numerical() == 0_a);

                    CHECK(model.get(Attr::Var::Column, x[1]).linear().get(ctr).numerical() == 0_a);
                    CHECK(model.get(Attr::Var::Column, x[1]).quadratic().get(ctr, x[0]).numerical() == 0_a);
                    CHECK(model.get(Attr::Var::Column, x[1]).quadratic().get(ctr, x[1]).numerical() == 0_a);
                    CHECK(model.get(Attr::Var::Column, x[1]).quadratic().get(ctr, x[2]).numerical() == 0_a);

                    CHECK(model.get(Attr::Var::Column, x[2]).linear().get(ctr).numerical() == 0_a);
                    CHECK(model.get(Attr::Var::Column, x[2]).quadratic().get(ctr, x[0]).numerical() == 0_a);
                    CHECK(model.get(Attr::Var::Column, x[2]).quadratic().get(ctr, x[1]).numerical() == 0_a);
                    CHECK(model.get(Attr::Var::Column, x[2]).quadratic().get(ctr, x[2]).numerical() == 0_a);
                }

            }

        }

    }

}