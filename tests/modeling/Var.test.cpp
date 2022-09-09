//
// Created by henri on 08/09/22.
//

#include "../test_utils.h"

TEST_CASE("Var", "[variables][modeling]") {

    Env env;
    Model model(env);

    auto xi = model.add_parameter(0., 1., Continuous);
    auto c1 = model.add_constraint(GreaterOrEqual, 0.);
    auto c2 = model.add_constraint(GreaterOrEqual, 0.);

    SECTION("create a new variable with double objective coefficient") {

        auto x = model.add_variable(0., 1., Continuous, 1.);

        CHECK(x.obj().constant() == 1.);
        CHECK(x.lb() == 0._a);
        CHECK(x.ub() == 1._a);
        CHECK(x.type() == Continuous);

    }

    SECTION("create a new variable with Param objective coefficient") {

        auto x = model.add_variable(0., 1., Continuous, xi);

        CHECK(x.obj().constant() == 0.);
        CHECK(x.obj().get(xi) == 1.);

    }

    SECTION("create a new variable with Coefficient objective coefficient") {

        auto x = model.add_variable(0., 1., Continuous, 1 + 2 * xi);

        CHECK(x.obj().constant() == 1.);
        CHECK(x.obj().get(xi) == 2.);

    }

    SECTION("create a new variable with Column") {

        Column column(xi);
        column.set(c1, 1 + 2 * xi);
        column.set(c2, 3);

        auto x = model.add_variable(0., 1., Continuous, column);

        CHECK(x.obj().constant() == 0._a);
        CHECK(x.obj().get(xi) == 1._a);
        CHECK(x.get(c1).constant() == 1._a);
        CHECK(x.get(c1).get(xi) == 2._a);
        CHECK(x.get(c2).constant() == 3._a);
        CHECK(c1.get(x).constant() == 1._a);
        CHECK(c1.get(x).get(xi) == 2._a);
        CHECK(c2.get(x).constant() == 3._a);

        column.set(c1, 0.);

        CHECK(x.get(c1).constant() == 1._a);

    }

    SECTION("create a new variable with type Integer") {

        auto x = model.add_variable(0., 1., Integer, 0);
        CHECK(x.type() == Integer);

    }

    SECTION("create a new variable with type Binary") {

        auto x = model.add_variable(0., 1., Binary, 0);
        CHECK(x.type() == Binary);

    }

    SECTION("update an existing variable") {

        auto x = model.add_variable(0., 1., Continuous, 0.);

        SECTION("update lb") {
            model.update_lb(x, -1.);
            CHECK(x.lb() == -1._a);
        }

        SECTION("update ub") {
            model.update_lb(x, 3.);
            CHECK(x.lb() == 3._a);
        }

        SECTION("update type") {

            SECTION("Continuous") {
                model.update_type(x, Continuous);
                CHECK(x.type() == Continuous);
            }

            SECTION("Integer") {
                model.update_type(x, Integer);
                CHECK(x.type() == Integer);
            }

            SECTION("Binary") {
                model.update_type(x, Binary);
                CHECK(x.type() == Binary);
            }
        }


    }

}