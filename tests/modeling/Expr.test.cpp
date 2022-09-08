//
// Created by henri on 08/09/22.
//
#include "../test_utils.h"

TEST_CASE("Expr", "[expressions][modeling]") {

    Env env;
    Model model(env);

    auto a = model.add_parameter(0., 1., Continuous);
    auto b = model.add_parameter(0., 1., Continuous);
    auto x = model.add_variable(0., 1., Continuous, 0.);
    auto y = model.add_variable(0., 1., Continuous, 0.);

    SECTION("constructor with no argument") {

        Expr expr;
        CHECK(expr.size() == 0);

    }

    SECTION("product between double and Var") {

        SECTION("with zero") {
            auto expr = 0 * x;

            CHECK(expr.size() == 0);
            CHECK(expr.get(x).constant() == 0._a);
        }

        SECTION("with a non-zero value") {
            auto expr = 2 * x;

            CHECK(expr.size() == 1);
            CHECK(expr.get(x).constant() == 2._a);
        }

    }

    SECTION("sum between Var and Var") {

        auto expr = x + y;

        CHECK(expr.size() == 2);
        CHECK(expr.get(x).constant() == 1._a);
        CHECK(expr.get(y).constant() == 1._a);

    }

    SECTION("sum between Expr and Var") {

        auto expr = 2 * x + y;

        CHECK(expr.size() == 2);
        CHECK(expr.get(x).constant() == 2._a);
        CHECK(expr.get(y).constant() == 1._a);

    }

    SECTION("sum between Var and Expr") {

        auto expr = x + 2 * y;

        CHECK(expr.size() == 2);
        CHECK(expr.get(x).constant() == 1._a);
        CHECK(expr.get(y).constant() == 2._a);

    }


    SECTION("sum between Var and Expr") {

        auto expr = 3 * x + 2 * y;

        CHECK(expr.size() == 2);
        CHECK(expr.get(x).constant() == 3._a);
        CHECK(expr.get(y).constant() == 2._a);

    }

    SECTION("product between Param and Var") {

        auto expr = a * x;
        CHECK(expr.size() == 1);
        CHECK(expr.get(x).constant() == 0._a);
        CHECK(expr.get(x).get(a) == 1._a);

    }

    SECTION("product between Coefficient and Var") {

        auto expr = (1 + 2 * a) * x;
        CHECK(expr.size() == 1);
        CHECK(expr.get(x).constant() == 1._a);
        CHECK(expr.get(x).get(a) == 2._a);

    }

    SECTION("build a complex expression") {

        auto expr = x + y + 2 * x + 3 * a * y + (2 + 3 * a + b) * x;

        CHECK(expr.size() == 2);
        CHECK(expr.get(x).constant() == 5._a);
        CHECK(expr.get(x).get(a) == 3._a);
        CHECK(expr.get(x).get(b) == 1._a);
        CHECK(expr.get(y).constant() == 1._a);
        CHECK(expr.get(y).get(a) == 3._a);

    }

}