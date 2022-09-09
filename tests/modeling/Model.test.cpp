//
// Created by henri on 09/09/22.
//
#include "../test_utils.h"

TEST_CASE("Model", "[model][modeling]") {

    Env env;
    Model model(env);

    SECTION("remove a variable") {
        auto x = model.add_variable(0., 1., Continuous, 0.);
        CHECK(model.variables().size() == 1);
        model.remove(x);
        CHECK(model.variables().size() == 0);
    }

    SECTION("remove a parameter") {
        auto xi = model.add_parameter(0., 1., Continuous);
        CHECK(model.parameters().size() == 1);
        model.remove(xi);
        CHECK(model.parameters().size() == 0);
    }

    SECTION("remove a constraint") {
        auto ctr = model.add_constraint(GreaterOrEqual, 0.);
        CHECK(model.constraints().size() == 1);
        model.remove(ctr);
        CHECK(model.constraints().size() == 0);
    }

    SECTION("iterate over variables") {

        model.add_variable(0., 1., Continuous, 0., "x");
        model.add_variable(0., 1., Continuous, 0., "y");

        bool x_has_been_found = false;
        bool y_has_been_found = false;

        for (const auto& var : model.variables()) {
            if (var.name() == "x") {
                x_has_been_found = true;
            } else if (var.name() == "y") {
                y_has_been_found = true;
            }
        }

        CHECK(x_has_been_found);
        CHECK(y_has_been_found);

    }

    SECTION("iterate over constraints") {

        model.add_constraint(GreaterOrEqual, 0., "c1");
        model.add_constraint(GreaterOrEqual, 0., "c2");

        bool c1_has_been_found = false;
        bool c2_has_been_found = false;

        for (const auto& ctr : model.constraints()) {
            if (ctr.name() == "c1") {
                c1_has_been_found = true;
            } else if (ctr.name() == "c2") {
                c2_has_been_found = true;
            }
        }

        CHECK(c1_has_been_found);
        CHECK(c2_has_been_found);

    }

    SECTION("iterate over parameters") {

        model.add_parameter(0., 1., Continuous, "x");
        model.add_parameter(0., 1., Continuous, "y");

        bool x_has_been_found = false;
        bool y_has_been_found = false;

        for (const auto& param : model.parameters()) {
            if (param.name() == "x") {
                x_has_been_found = true;
            } else if (param.name() == "y") {
                y_has_been_found = true;
            }
        }

        CHECK(x_has_been_found);
        CHECK(y_has_been_found);

    }

    SECTION("update coefficient") {

        auto x = model.add_variable(0., 1., Continuous, 0.);
        auto y = model.add_variable(0., 1., Continuous, 0.);
        auto c1 = model.add_constraint(x + y >= 1);
        auto c2 = model.add_constraint(x <= 1);

        SECTION("insert a new coefficient") {

            SECTION("with a non-zero coefficient") {

                model.update_coefficient(c2, y, 2.);

                CHECK(c2.get(x).constant() == 1._a);
                CHECK(c2.row().get(x).constant() == 1._a);
                CHECK(c2.get(y).constant() == 2._a);
                CHECK(c2.row().get(y).constant() == 2._a);

                CHECK(x.get(c2).constant() == 1._a);
                CHECK(x.column().get(c2).constant() == 1._a);
                CHECK(y.get(c2).constant() == 2._a);
                CHECK(y.column().get(c2).constant() == 2._a);

                CHECK(c2.row().size() == 2);
                CHECK(y.column().size() == 2);

            }

            SECTION("with a non-zero coefficient") {

                model.update_coefficient(c2, y, 0.);

                CHECK(c2.get(x).constant() == 1._a);
                CHECK(c2.row().get(x).constant() == 1._a);
                CHECK(c2.get(y).constant() == 0._a);
                CHECK(c2.row().get(y).constant() == 0._a);

                CHECK(x.get(c2).constant() == 1._a);
                CHECK(x.column().get(c2).constant() == 1._a);
                CHECK(y.get(c2).constant() == 0._a);
                CHECK(y.column().get(c2).constant() == 0._a);

                CHECK(c2.row().size() == 1);
                CHECK(y.column().size() == 1);

            }

        }

        SECTION("update a new coefficient") {

            SECTION("with a non-zero coefficient") {

                model.update_coefficient(c2, x, 2.);

                CHECK(c2.get(x).constant() == 2._a);
                CHECK(c2.row().get(x).constant() == 2._a);
                CHECK(c2.get(y).constant() == 0._a);
                CHECK(c2.row().get(y).constant() == 0._a);

                CHECK(x.get(c2).constant() == 2._a);
                CHECK(x.column().get(c2).constant() == 2._a);
                CHECK(y.get(c2).constant() == 0._a);
                CHECK(y.column().get(c2).constant() == 0._a);

                CHECK(c2.row().size() == 1);
                CHECK(x.column().size() == 2);

            }

            SECTION("with a non-zero coefficient") {

                model.update_coefficient(c2, x, 0.);

                CHECK(c2.get(x).constant() == 0._a);
                CHECK(c2.row().get(x).constant() == 0._a);
                CHECK(c2.get(y).constant() == 0._a);
                CHECK(c2.row().get(y).constant() == 0._a);

                CHECK(x.get(c2).constant() == 0._a);
                CHECK(x.column().get(c2).constant() == 0._a);
                CHECK(y.get(c2).constant() == 0._a);
                CHECK(y.column().get(c2).constant() == 0._a);

                CHECK(c2.row().size() == 0);
                CHECK(x.column().size() == 1);

            }

        }

    }

}