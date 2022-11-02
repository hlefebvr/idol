//
// Created by henri on 09/09/22.
//

#include "../test_utils.h"

TEST_CASE("03. Var", "[attributes][modeling]") {

    Model sp;
    Model model;

    SECTION("create a new parameter with type Continuous") {

        auto xi = Param(sp.add_var(0., 1., Continuous, 0.) );

        CHECK(xi.as<Var>().lb() == 0._a);
        CHECK(xi.as<Var>().ub() == 1._a);
        CHECK(xi.as<Var>().type() == Continuous);

    }

    SECTION("create a new parameter with type Integer") {

        auto xi = Param(sp.add_var(0., 1., Integer, 0.) );
        CHECK(xi.as<Var>().type() == Integer);

    }

    SECTION("create a new parameter with type Binary") {

        auto xi = Param(sp.add_var(0., 1., Binary, 0.) );
        CHECK(xi.as<Var>().type() == Binary);

    }

}