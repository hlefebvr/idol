//
// Created by henri on 09/09/22.
//

#include "../test_utils.h"

TEST_CASE("Var", "[parameters][modeling]") {

    Env env;
    Model sp(env);
    Model model(env);

    SECTION("create a new parameter with type Continuous") {

        auto xi = model.add_parameter( sp.add_variable(0., 1., Continuous, 0.) );

        CHECK(xi.lb() == 0._a);
        CHECK(xi.ub() == 1._a);
        CHECK(xi.type() == Continuous);

    }

    SECTION("create a new parameter with type Integer") {

        auto xi = model.add_parameter( sp.add_variable(0., 1., Integer, 0.) );
        CHECK(xi.type() == Integer);

    }

    SECTION("create a new parameter with type Binary") {

        auto xi = model.add_parameter( sp.add_variable(0., 1., Binary, 0.) );
        CHECK(xi.type() == Binary);

    }

}