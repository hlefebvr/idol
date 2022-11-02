//
// Created by henri on 08/09/22.
//

#include "../test_utils.h"

TEST_CASE("02. Ctr", "[constraints][modeling]") {

    Model sp;
    Model model;

    auto xi = Param( sp.add_variable(0., 1., Continuous, 0.) );
    auto x = model.add_variable(0., 1., Continuous, 0.);
    auto y = model.add_variable(0., 1., Continuous, 0.);

    SECTION("create a new constraint with double RHS") {

        auto ctr = model.add_constraint(GreaterOrEqual, 1.);

        CHECK(ctr.type() == GreaterOrEqual);
        CHECK(ctr.rhs().numerical() == 1._a);
        CHECK(ctr.model_id() == model.id());

    }

    SECTION("create a new constraint with Param RHS") {

        auto ctr = model.add_constraint(LessOrEqual, xi);

        CHECK(ctr.type() == LessOrEqual);
        CHECK(ctr.rhs().numerical() == 0._a);
        CHECK(ctr.rhs().get(xi) == 1._a);

    }

    SECTION("create a new constraint with Coefficient RHS") {

        auto ctr = model.add_constraint(Equal, 1 + 2 * xi);

        CHECK(ctr.type() == Equal);
        CHECK(ctr.rhs().numerical() == 1._a);
        CHECK(ctr.rhs().get(xi) == 2._a);

    }

    SECTION("create a new constraint with Expr") {

        auto ctr = model.add_constraint(2 * x + y <= 2 + xi);

        CHECK(ctr.type() == LessOrEqual);
        CHECK(ctr.rhs().numerical() == 2._a);
        CHECK(ctr.rhs().get(xi) == 1._a);
        CHECK(ctr.get(x).numerical() == 2._a);
        CHECK(ctr.get(y).numerical() == 1._a);
        CHECK(x.get(ctr).numerical() == 2._a);
        CHECK(y.get(ctr).numerical() == 1._a);

    }

    SECTION("update an existing constraint") {

        auto ctr = model.add_constraint(x + y <= 2);

        SECTION("update RHS") {

            model.update_rhs_coeff(ctr, 0.);

            CHECK(ctr.rhs().numerical() == 0._a);
            CHECK(ctr.rhs().get(xi) == 0._a);

            model.update_rhs_coeff(ctr, 1 + 2 * xi);

            CHECK(ctr.rhs().numerical() == 1._a);
            CHECK(ctr.rhs().get(xi) == 2._a);

        }

        SECTION("update type") {

            SECTION("GreaterOrEqual") {
                model.update_ctr_type(ctr, GreaterOrEqual);
                CHECK(ctr.type() == GreaterOrEqual);
            }

            SECTION("LessOrEqual") {
                model.update_ctr_type(ctr, LessOrEqual);
                CHECK(ctr.type() == LessOrEqual);
            }

            SECTION("Equal") {
                model.update_ctr_type(ctr, Equal);
                CHECK(ctr.type() == Equal);
            }

        }

    }

}