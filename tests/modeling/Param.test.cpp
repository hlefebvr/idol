//
// Created by henri on 09/09/22.
//

#include "../test_utils.h"

TEST_CASE("Var", "[parameters][modeling]") {

    Env env;
    Model model(env);

    SECTION("create a new parameter with type Continuous") {

        auto xi = model.add_parameter(0., 1., Continuous);

        CHECK(xi.lb() == 0._a);
        CHECK(xi.ub() == 1._a);
        CHECK(xi.type() == Continuous);

    }

    SECTION("create a new parameter with type Integer") {

        auto xi = model.add_parameter(0., 1., Integer);
        CHECK(xi.type() == Integer);

    }

    SECTION("create a new parameter with type Binary") {

        auto xi = model.add_parameter(0., 1., Binary);
        CHECK(xi.type() == Binary);

    }

    SECTION("update an exiisting parameter") {

        auto xi = model.add_parameter(0., 1., Continuous);

        SECTION("update lb") {
            model.update_lb(xi, -1.);
            CHECK(xi.lb() == -1._a);
        }

        SECTION("update ub") {
            model.update_ub(xi, 3.);
            CHECK(xi.ub() == 3._a);
        }

        SECTION("update type") {

            SECTION("Continuous") {
                model.update_type(xi, Continuous);
                CHECK(xi.type() == Continuous);
            }

            SECTION("Integer") {
                model.update_type(xi, Integer);
                CHECK(xi.type() == Integer);
            }

            SECTION("Binary") {
                model.update_type(xi, Binary);
                CHECK(xi.type() == Binary);
            }
        }


    }

}