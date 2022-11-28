//
// Created by henri on 08/09/22.
//

#include "../test_utils.h"

TEST_CASE("02. Ctr", "[constraints][modeling]") {

    Model sp;
    Model model;

    auto xi = InParam(sp.add_var(0., 1., Continuous, 0.) );
    auto x = model.add_var(0., 1., Continuous, 0.);
    auto y = model.add_var(0., 1., Continuous, 0.);

    SECTION("create a new constraint with double RHS") {

        auto ctr = model.add_ctr(GreaterOrEqual, 1.);

        CHECK(model.get(Attr::Ctr::Type, ctr) == GreaterOrEqual);
        CHECK(model.get(Attr::Ctr::Row, ctr).rhs().numerical() == 1._a);

    }

    SECTION("create a new constraint with Param RHS") {

        auto ctr = model.add_ctr(LessOrEqual, xi);

        CHECK(model.get(Attr::Ctr::Type, ctr) == LessOrEqual);
        CHECK(model.get(Attr::Ctr::Row, ctr).rhs().numerical() == 0._a);
        CHECK(model.get(Attr::Ctr::Row, ctr).rhs().get(xi) == 1._a);

    }

    SECTION("create a new constraint with Coefficient RHS") {

        auto ctr = model.add_ctr(Equal, 1 + 2 * xi);

        CHECK(model.get(Attr::Ctr::Type, ctr) == Equal);
        CHECK(model.get(Attr::Ctr::Row, ctr).rhs().numerical() == 1._a);
        CHECK(model.get(Attr::Ctr::Row, ctr).rhs().get(xi) == 2._a);

    }

    SECTION("create a new constraint with Expr") {

        auto ctr = model.add_ctr(2 * x + y <= 2 + xi);

        CHECK(model.get(Attr::Ctr::Type, ctr) == LessOrEqual);
        CHECK(model.get(Attr::Ctr::Row, ctr).rhs().numerical() == 2._a);
        CHECK(model.get(Attr::Ctr::Row, ctr).rhs().get(xi) == 1._a);
        CHECK(model.get(Attr::Var::Column, x).linear().get(ctr).numerical() == 2._a);
        CHECK(model.get(Attr::Var::Column, y).linear().get(ctr).numerical() == 1._a);

    }

    SECTION("update an existing constraint") {

        auto ctr = model.add_ctr(x + y <= 2);

        SECTION("update RHS") {

            model.set(Attr::Ctr::Rhs, ctr, 0.);

            CHECK(model.get(Attr::Ctr::Row, ctr).rhs().numerical() == 0._a);
            CHECK(model.get(Attr::Ctr::Row, ctr).rhs().get(xi) == 0._a);

            model.set(Attr::Ctr::Rhs, ctr, 1 + 2 * xi);

            CHECK(model.get(Attr::Ctr::Row, ctr).rhs().numerical() == 1._a);
            CHECK(model.get(Attr::Ctr::Row, ctr).rhs().get(xi) == 2._a);

        }

        SECTION("update type") {

            SECTION("GreaterOrEqual") {
                model.set(Attr::Ctr::Type, ctr, GreaterOrEqual);
                CHECK(model.get(Attr::Ctr::Type, ctr) == GreaterOrEqual);
            }

            SECTION("LessOrEqual") {
                model.set(Attr::Ctr::Type, ctr, LessOrEqual);
                CHECK(model.get(Attr::Ctr::Type, ctr) == LessOrEqual);
            }

            SECTION("Equal") {
                model.set(Attr::Ctr::Type, ctr, Equal);
                CHECK(model.get(Attr::Ctr::Type, ctr) == Equal);
            }

        }

    }

}