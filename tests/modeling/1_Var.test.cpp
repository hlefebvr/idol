//
// Created by henri on 08/09/22.
//

#include "../test_utils.h"

TEST_CASE("01. Var", "[variables][modeling]") {

    Model sp;
    Model model;

    auto xi = InParam(sp.add_var(0., 1., Continuous, 0.) );
    auto c1 = model.add_ctr(GreaterOrEqual, 0.);
    auto c2 = model.add_ctr(GreaterOrEqual, 0.);

    SECTION("create a new variable with double objective coefficient") {

        auto x = model.add_var(0., 1., Continuous, 1.);

        CHECK(model.get(Attr::Var::Column, x).obj().numerical() == 1.);
        CHECK(model.get(Attr::Var::Lb, x) == 0._a);
        CHECK(model.get(Attr::Var::Ub, x) == 1._a);
        CHECK(model.get(Attr::Var::Type, x) == Continuous);

    }

    SECTION("create a new variable with Param objective coefficient") {

        auto x = model.add_var(0., 1., Continuous, xi);

        CHECK(model.get(Attr::Var::Column, x).obj().numerical() == 0.);
        CHECK(model.get(Attr::Var::Column, x).obj().get(xi) == 1.);

    }

    SECTION("create a new variable with Coefficient objective coefficient") {

        auto x = model.add_var(0., 1., Continuous, 1 + 2 * xi);

        CHECK(model.get(Attr::Var::Column, x).obj().numerical() == 1.);
        CHECK(model.get(Attr::Var::Column, x).obj().get(xi) == 2.);

    }

    SECTION("create a new variable with Column") {

        Column column(xi);
        column.linear().set(c1, 1 + 2 * xi);
        column.linear().set(c2, 3);

        auto x = model.add_var(0., 1., Continuous, column);

        CHECK(model.get(Attr::Var::Column, x).obj().numerical() == 0._a);
        CHECK(model.get(Attr::Var::Column, x).obj().get(xi) == 1._a);
        CHECK(model.get(Attr::Var::Column, x).linear().get(c1).numerical() == 1._a);
        CHECK(model.get(Attr::Var::Column, x).linear().get(c1).get(xi) == 2._a);
        CHECK(model.get(Attr::Var::Column, x).linear().get(c2).numerical() == 3._a);
        CHECK(model.get(Attr::Ctr::Row, c1).linear().get(x).numerical() == 1._a);
        CHECK(model.get(Attr::Ctr::Row, c1).linear().get(x).get(xi) == 2._a);
        CHECK(model.get(Attr::Ctr::Row, c2).linear().get(x).numerical() == 3._a);

        column.linear().set(c1, 0.);

        CHECK(model.get(Attr::Var::Column, x).linear().get(c1).numerical() == 1._a);

    }

    SECTION("create a new variable with type Integer") {

        auto x = model.add_var(0., 1., Integer, 0);
        CHECK(model.get(Attr::Var::Type, x) == Integer);

    }

    SECTION("create a new variable with type Binary") {

        auto x = model.add_var(0., 1., Binary, 0);
        CHECK(model.get(Attr::Var::Type, x) == Binary);

    }

    SECTION("update an existing variable") {

        auto x = model.add_var(0., 1., Continuous, 0.);

        SECTION("update lb") {
            model.set(Attr::Var::Lb, x, -1.);
            CHECK(model.get(Attr::Var::Lb, x) == -1._a);
        }

        SECTION("update ub") {
            model.set(Attr::Var::Ub, x, 3.);
            CHECK(model.get(Attr::Var::Ub, x) == 3._a);
        }

        SECTION("update type") {

            SECTION("Continuous") {
                model.set(Attr::Var::Type, x, Continuous);
                CHECK(model.get(Attr::Var::Type, x) == Continuous);
            }

            SECTION("Integer") {
                model.set(Attr::Var::Type, x, Integer);
                CHECK(model.get(Attr::Var::Type, x) == Integer);
            }

            SECTION("Binary") {
                model.set(Attr::Var::Type, x, Binary);
                CHECK(model.get(Attr::Var::Type, x) == Binary);
            }
        }


    }

    SECTION("cout") {

        std::stringstream stream;

        SECTION("with a given name") {
            auto x = model.add_var(0., 1., Continuous, 0., "x");
            stream << x;
            CHECK(stream.str() == "x");
        }

        SECTION("without any given name") {
            auto x = model.add_var(0., 1., Continuous, 0.);
            stream << x;
            CHECK(stream.str() == "Var__" + std::to_string(x.id()));
        }

    }

}