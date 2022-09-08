//
// Created by henri on 08/09/22.
//
#include "../test_utils.h"

TEST_CASE("Coefficient", "[expressions][modeling]") {

    Env env;
    Model model(env);

    auto a = model.add_parameter(0., 1., Continuous);
    auto b = model.add_parameter(0., 1., Continuous);

    SECTION("constructor with no argument") {

        Coefficient expr;

        CHECK(expr.size() == 0);
        CHECK(expr.constant() == 0.);

    }

    SECTION("constructor with double") {

        Coefficient expr(1.);

        CHECK(expr.size() == 0);
        CHECK(expr.constant() == 1.);

    }

    SECTION("constructor with Param and double") {

        SECTION("with a non-zero value") {

            Coefficient expr(a, 1.);
            
            CHECK(expr.size() == 1);
            CHECK(expr.constant() == 0.);
            CHECK(expr.get(a) == 1._a);

        }

        SECTION("with zero") {

            Coefficient expr(a, 0.);

            CHECK(expr.size() == 0);
            CHECK(expr.constant() == 0.);
            CHECK(expr.get(a) == 0._a);

        }

    }

    SECTION("product between double and Param") {

        auto expr = 2 * a;

        CHECK(expr.constant() == 0._a);
        CHECK(expr.get(a) == 2._a);
        CHECK(expr.size() == 1);

    }

    SECTION("product between double and Param") {

        auto expr = a * 2;

        CHECK(expr.constant() == 0._a);
        CHECK(expr.get(a) == 2._a);
        CHECK(expr.size() == 1);

    }

    SECTION("product between zero and Param") {

        auto expr = 0 * a;

        CHECK(expr.constant() == 0._a);
        CHECK(expr.get(a) == 0._a);
        CHECK(expr.size() == 0);

    }

    SECTION("product between Param and zero") {

        auto expr = a * 0;

        CHECK(expr.constant() == 0._a);
        CHECK(expr.get(a) == 0._a);
        CHECK(expr.size() == 0);

    }

    SECTION("sum between double and Param") {

        auto expr = 2 + a;

        CHECK(expr.constant() == 2._a);
        CHECK(expr.get(a) == 1._a);
        CHECK(expr.size() == 1);

    }

    SECTION("sum between Param and double") {

        auto expr = a + 2;

        CHECK(expr.constant() == 2._a);
        CHECK(expr.get(a) == 1._a);
        CHECK(expr.size() == 1);

    }

    SECTION("sum between Param and Param") {

        auto expr = a + b;

        CHECK(expr.constant() == 0._a);
        CHECK(expr.get(a) == 1._a);
        CHECK(expr.get(b) == 1._a);
        CHECK(expr.size() == 2);

    }

    SECTION("sum between Param and Coefficient") {

        auto expr = a + 2 * b;

        CHECK(expr.constant() == 0._a);
        CHECK(expr.get(a) == 1._a);
        CHECK(expr.get(b) == 2._a);
        CHECK(expr.size() == 2);

    }

    SECTION("sum between Coefficient and Param") {

        auto expr = 2 * a + b;

        CHECK(expr.constant() == 0._a);
        CHECK(expr.get(a) == 2._a);
        CHECK(expr.get(b) == 1._a);
        CHECK(expr.size() == 2);

    }

    SECTION("sum between Coefficient and Coefficient") {

        auto expr = 9 + 2 * a + 3 * b;

        CHECK(expr.constant() == 9._a);
        CHECK(expr.get(a) == 2._a);
        CHECK(expr.get(b) == 3._a);
        CHECK(expr.size() == 2);

    }

    SECTION("build a complex expression") {

        auto expr = 2 * a + b + a + 3 * (a + b + 2) * 10;

        CHECK(expr.constant() == 60._a);
        CHECK(expr.get(a) == 33._a);
        CHECK(expr.get(b) == 31._a);
        CHECK(expr.size() == 2);

    }

    SECTION("set non-zero coefficient") {

        Coefficient expr;
        expr.set(a, 3);

        CHECK(expr.get(a) == 3._a);
        CHECK(expr.size() == 1);

    }

    SECTION("set zero coefficient") {

        Coefficient expr;
        expr.set(a, 0);

        CHECK(expr.get(a) == 0._a);
        CHECK(expr.size() == 0);

    }

    SECTION("remove non-zero coefficient by setting it to zero") {

        Coefficient expr = 3 * a + b;

        expr.set(a, 0.);

        CHECK(expr.get(a) == 0._a);
        CHECK(expr.size() == 1);

    }

    SECTION("set constant") {

        auto expr = 10. + a;

        expr.set_constant(0.);

        CHECK(expr.constant() == 0.);

    }
}