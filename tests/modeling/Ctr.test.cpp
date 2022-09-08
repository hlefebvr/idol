//
// Created by henri on 08/09/22.
//

#include "../test_utils.h"

TEST_CASE("Ctr", "[constraints][modeling]") {

    Env env;
    Model model(env);

    auto xi = model.add_parameter(0., 1., Continuous);
    auto x = model.add_variable(0., 1., Continuous, 0.);
    auto y = model.add_variable(0., 1., Continuous, 0.);

    SECTION("create a new constraint with double RHS") {

        auto ctr = model.add_constraint(GreaterOrEqual, 1.);

        CHECK(ctr.type() == GreaterOrEqual);
        CHECK(ctr.rhs().constant() == 1._a);

    }

    SECTION("create a new constraint with Param RHS") {

        auto ctr = model.add_constraint(LessOrEqual, xi);

        CHECK(ctr.type() == LessOrEqual);
        CHECK(ctr.rhs().constant() == 0._a);
        CHECK(ctr.rhs().get(xi) == 1._a);

    }

    SECTION("create a new constraint with Coefficient RHS") {

        auto ctr = model.add_constraint(Equal, 1 + 2 * xi);

        CHECK(ctr.type() == Equal);
        CHECK(ctr.rhs().constant() == 1._a);
        CHECK(ctr.rhs().get(xi) == 2._a);

    }

    SECTION("create a new constraint with Expr") {

        auto ctr = model.add_constraint(2 * x + y <= 2 + xi);

        CHECK(ctr.type() == LessOrEqual);
        CHECK(ctr.rhs().constant() == 2._a);
        CHECK(ctr.rhs().get(xi) == 1._a);
        CHECK(ctr.get(x).constant() == 2._a);
        CHECK(ctr.get(y).constant() == 1._a);
        CHECK(x.get(ctr).constant() == 2._a);
        CHECK(y.get(ctr).constant() == 1._a);

    }

}