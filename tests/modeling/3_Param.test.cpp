//
// Created by henri on 09/09/22.
//

#include "../test_utils.h"

TEST_CASE("03. Param", "[attributes][modeling]") {

    Model model;

    SECTION("create a new parameter with type Continuous") {

        auto xi = InParam(model.add_var(0., 1., Continuous, 0.) );

        CHECK(model.get(Attr::Var::Lb, xi.as<Var>()) == 0._a);
        CHECK(model.get(Attr::Var::Ub, xi.as<Var>()) == 1._a);
        CHECK(model.get(Attr::Var::Type, xi.as<Var>()) == Continuous);

    }

    SECTION("create a new parameter with type Integer") {

        auto xi = InParam(model.add_var(0., 1., Integer, 0.) );
        CHECK(model.get(Attr::Var::Type, xi.as<Var>()) == Integer);

    }

    SECTION("create a new parameter with type Binary") {

        auto xi = InParam(model.add_var(0., 1., Binary, 0.) );
        CHECK(model.get(Attr::Var::Type, xi.as<Var>()) == Binary);

    }

}