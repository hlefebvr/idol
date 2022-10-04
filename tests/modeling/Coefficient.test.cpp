//
// Created by henri on 08/09/22.
//
#include "../test_utils.h"

TEST_CASE("Coefficient", "[expressions][modeling]") {

    Env env;
    Model sp(env);
    Model model(env);

    auto a = model.add_parameter( sp.add_variable(0., 1., Continuous, 0.) );
    auto b = model.add_parameter( sp.add_variable(0., 1., Continuous, 0.) );

    SECTION("constructor with no argument") {

        Constant expr;

        CHECK(expr.size() == 0);
        CHECK(expr.constant() == 0.);

    }

    SECTION("constructor with double") {

        Constant expr(1.);

        CHECK(expr.size() == 0);
        CHECK(expr.constant() == 1.);

    }

    SECTION("constructor with Param and double") {

        SECTION("with a non-zero value") {

            Constant expr(a, 1.);
            
            CHECK(expr.size() == 1);
            CHECK(expr.constant() == 0.);
            CHECK(expr.get(a) == 1._a);

        }

        SECTION("with zero") {

            Constant expr(a, 0.);

            CHECK(expr.size() == 0);
            CHECK(expr.constant() == 0.);
            CHECK(expr.get(a) == 0._a);

        }

    }

    SECTION("product between double and Param") {

        SECTION("with a non-zero value") {

            auto expr = 2 * a;

            CHECK(expr.constant() == 0._a);
            CHECK(expr.get(a) == 2._a);
            CHECK(expr.size() == 1);

        }

        SECTION("with zero") {

            auto expr = 0 * a;

            CHECK(expr.constant() == 0._a);
            CHECK(expr.get(a) == 0._a);
            CHECK(expr.size() == 0);

        }

    }

    SECTION("product between double and Coefficient") {

        SECTION("with a non-zero value") {

            auto expr = 10 * (1 + 2 * a);

            CHECK(expr.constant() == 10._a);
            CHECK(expr.get(a) == 20._a);
            CHECK(expr.size() == 1);

        }

        SECTION("with zero") {

            auto expr = 0 * (1 + 2 * a);

            CHECK(expr.constant() == 0._a);
            CHECK(expr.get(a) == 0._a);
            CHECK(expr.size() == 0);

        }

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

    SECTION("auto sum with double") {
        auto expr = 1 + 2 * a;
        expr += 1.;
        CHECK(expr.constant() == 2._a);
    }

    SECTION("auto sum with Param") {
        auto expr = 1 + 2 * a;
        expr += a;
        CHECK(expr.constant() == 1._a);
        CHECK(expr.get(a) == 3._a);
    }

    SECTION("sum with opposite Coefficient") {
        auto e1 = 2 * a;
        auto e2 = -2 * a;
        auto expr = e1 + e2;
        CHECK(expr.is_zero());
        CHECK(expr.size() == 0);
    }

    SECTION("build a complex expression") {

        auto expr = 2 * a + b + a + 3 * 10 * (a + b + 2);

        CHECK(expr.constant() == 60._a);
        CHECK(expr.get(a) == 33._a);
        CHECK(expr.get(b) == 31._a);
        CHECK(expr.size() == 2);

    }

    SECTION("set non-zero coefficient") {

        Constant expr;
        expr.set(a, 3);

        CHECK(expr.get(a) == 3._a);
        CHECK(expr.size() == 1);

    }

    SECTION("update a non-zero coefficient with a non-zero coefficient") {
        auto expr = 2 * a;
        expr.set(a, 3);
        CHECK(expr.get(a) == 3._a);
    }

    SECTION("set zero coefficient") {

        Constant expr;
        expr.set(a, 0);

        CHECK(expr.get(a) == 0._a);
        CHECK(expr.size() == 0);

    }

    SECTION("remove non-zero coefficient by setting it to zero") {

        Constant expr = 3 * a + b;

        expr.set(a, 0.);

        CHECK(expr.get(a) == 0._a);
        CHECK(expr.size() == 1);

    }

    SECTION("set constant") {

        auto expr = 10. + a;

        expr.set_constant(0.);

        CHECK(expr.constant() == 0.);

    }

    SECTION("is_numerical") {

        auto expr = 1 + 2 * a;
        CHECK(!expr.is_numerical());
        expr = 10;
        CHECK(expr.is_numerical());

    }
}