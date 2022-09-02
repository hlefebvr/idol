//
// Created by henri on 02/09/22.
//

#ifndef OPTIMIZE_LINEXPR_TEST_CPP
#define OPTIMIZE_LINEXPR_TEST_CPP

#include "../test_utils.h"

TEMPLATE_LIST_TEST_CASE("LinExpr", "[expressions][modeling]", for_each_player) {

    Env env;
    Model<TestType::PlayerT> model(env);
    auto x = model.add_variable(0., Inf, Continuous, "x");
    auto y = model.add_variable(0., Inf, Continuous, "y");

    SECTION("should do the product between (double, Variable<PlayerT>)") {

        SECTION("with explicit type") {
            double c = 3.;
            auto lin_expr = c * x;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 3._a);
        }

        SECTION("with deduced type") {
            auto lin_expr = 3 * x;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 3._a);
        }

    }

    SECTION("should do the product between (Variable<PlayerT>, double)") {

        SECTION("with explicit type") {
            double c = 3.;
            auto lin_expr = x * c;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 3._a);
        }

        SECTION("with deduced type") {
            auto lin_expr = x * 3;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 3._a);
        }

    }

    SECTION("should do the sum between (double, Variable<PlayerT>)") {

        SECTION("with explicit type") {
            double c = 3.;
            auto lin_expr = c + x;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 1._a);
            CHECK(lin_expr.constant() == 3.);
        }

        SECTION("with deduced type") {
            auto lin_expr = 3 + x;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 1._a);
            CHECK(lin_expr.constant() == 3.);
        }

    }

    SECTION("should do the sum between (Variable<PlayerT>, double)") {

        SECTION("with explicit type") {
            double c = 3.;
            auto lin_expr = x + c;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 1._a);
            CHECK(lin_expr.constant() == 3._a);
        }

        SECTION("with deduced type") {
            auto lin_expr = x + 3;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 1._a);
            CHECK(lin_expr.constant() == 3._a);
        }

    }

    SECTION("should do the sum between (Variable<PlayerT>, Variable<PlayerT>)") {

        SECTION("\"with explicit type\" with different variables") {

            auto lin_expr = y + x;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 1._a);
            CHECK(lin_expr[y] == 1._a);

        }

        SECTION("\"with explicit type\" with same variables") {
            auto lin_expr = x + x;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 2._a);
        }

    }

    SECTION("should do the sum between (Variable<PlayerT>, LinExpr<PlayerT>)") {

        SECTION("with same variables") {

            SECTION("with explicit type") {
                LinExpr<TestType::PlayerT> c = 2 * x;
                auto lin_expr = x + c;
                CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
                CHECK(lin_expr[x] == 3._a);
            }

            SECTION("with deduced type") {
                auto lin_expr = x + 2 * x;
                CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
                CHECK(lin_expr[x] == 3._a);
            }

        }

        SECTION("with different variables") {

            SECTION("with explicit type") {
                LinExpr<TestType::PlayerT> c = 2 * y;
                auto lin_expr = x + c;
                CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
                CHECK(lin_expr[x] == 1._a);
                CHECK(lin_expr[y] == 2._a);
            }

            SECTION("with deduced type") {
                auto lin_expr = x + 2 * y;
                CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
                CHECK(lin_expr[x] == 1._a);
                CHECK(lin_expr[y] == 2._a);
            }

        }

    }

    SECTION("should do the sum between (LinExpr<PlayerT>, Variable<PlayerT>)") {

        SECTION("with same variables") {

            SECTION("with explicit type") {
                LinExpr<TestType::PlayerT> c = 2 * x;
                auto lin_expr = c + x;
                CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
                CHECK(lin_expr[x] == 3._a);
            }

            SECTION("with deduced type") {
                auto lin_expr = 2 * x + x;
                CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
                CHECK(lin_expr[x] == 3._a);
            }

        }

        SECTION("with different variables") {

            SECTION("with explicit type") {
                LinExpr<TestType::PlayerT> c = 2 * y;
                auto lin_expr = c + x;
                CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
                CHECK(lin_expr[x] == 1._a);
                CHECK(lin_expr[y] == 2._a);
            }

            SECTION("with deduced type") {
                auto lin_expr = 2 * y + x;
                CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
                CHECK(lin_expr[x] == 1._a);
                CHECK(lin_expr[y] == 2._a);
            }

        }

    }

    SECTION("should do the sum between (double, LinExpr<PlayerT>)") {

        SECTION("with explicit type") {
            LinExpr<TestType::PlayerT> c = 2 * x;
            double d = 1.;
            auto lin_expr = d + c;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 2._a);
            CHECK(lin_expr.constant() == 1._a);
        }

        SECTION("with deduced type") {
            auto lin_expr = 1. + 2 * x;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 2._a);
            CHECK(lin_expr.constant() == 1._a);
        }

    }

    SECTION("should do the sum between (LinExpr<PlayerT>, double)") {

        SECTION("with explicit type") {
            LinExpr<TestType::PlayerT> c = 2 * x;
            double d = 1.;
            auto lin_expr = c + d;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 2._a);
            CHECK(lin_expr.constant() == 1._a);
        }

        SECTION("with deduced type") {
            auto lin_expr = 2 * x + 1;
            CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
            CHECK(lin_expr[x] == 2._a);
            CHECK(lin_expr.constant() == 1._a);
        }


    }

    SECTION("should do the sum between (LinExpr<PlayerT>, LinExpr<PlayerT>)") {

        SECTION("with same variables") {

            SECTION("with explicit type") {
                LinExpr<TestType::PlayerT> a = 2 * x;
                LinExpr<TestType::PlayerT> b = 3 * x;
                auto lin_expr = a + b;
                CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
                CHECK(lin_expr[x] == 5._a);
            }

            SECTION("with deduced type") {
                auto lin_expr = 2 * x + 3 * x;
                CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
                CHECK(lin_expr[x] == 5._a);
            }

        }

        SECTION("with different variables") {

            SECTION("with explicit type") {
                LinExpr<TestType::PlayerT> a = 2 * x;
                LinExpr<TestType::PlayerT> b = 3 * y;
                auto lin_expr = a + b;
                CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
                CHECK(lin_expr[x] == 2._a);
                CHECK(lin_expr[y] == 3._a);
            }

            SECTION("with deduced type") {
                auto lin_expr = 2 * x + 3 * y;
                CHECK(std::is_same_v<LinExpr<TestType::PlayerT>, decltype(lin_expr)>);
                CHECK(lin_expr[x] == 2._a);
                CHECK(lin_expr[y] == 3._a);
            }

        }

    }

    SECTION("should construct with ()") {
        LinExpr<TestType::PlayerT> lin_expr;
        CHECK(lin_expr.n_terms() == 0);
        CHECK(lin_expr.constant() == 0._a);
        CHECK(lin_expr.is_numerical());
    }

    SECTION("should construct with (double)") {

        SECTION("with explicit type") {
            double c = 3.;
            LinExpr<TestType::PlayerT> lin_expr(c);
            CHECK(lin_expr.n_terms() == 0);
            CHECK(lin_expr.constant() == 3._a);
        }

        SECTION("with deduced type") {
            LinExpr<TestType::PlayerT> lin_expr(3);
            CHECK(lin_expr.n_terms() == 0);
            CHECK(lin_expr.constant() == 3._a);
        }

    }

    SECTION("should construct with (Variable<PlayerT>)") {

        LinExpr<TestType::PlayerT> lin_expr(x);
        CHECK(lin_expr.n_terms() == 1);
        CHECK(lin_expr[x] == 1._a);
        CHECK(lin_expr.constant() == 0._a);

    }

    SECTION("should construct with (const LinExpr<PlayerT>&)") {

        LinExpr<TestType::PlayerT> c = 2 * x;
        LinExpr<TestType::PlayerT> lin_expr(c);
        CHECK(lin_expr[x] == 2._a);
        CHECK(lin_expr.constant() == 0._a);

    }

    SECTION("should construct with (LinExpr<PlayerT>&&)") {


        SECTION("with explicit type") {
            LinExpr<TestType::PlayerT> c = 2 * x;
            LinExpr<TestType::PlayerT> lin_expr(std::move(c));
            CHECK(lin_expr[x] == 2._a);
            CHECK(lin_expr.constant() == 0._a);
        }

        SECTION("with deduced type") {
            LinExpr<TestType::PlayerT> lin_expr(2 * x);
            CHECK(lin_expr[x] == 2._a);
            CHECK(lin_expr.constant() == 0._a);
        }

    }

    SECTION("should make equality with (double)") {

        SECTION("with explicit type") {
            double c = 3.;
            LinExpr<TestType::PlayerT> lin_expr = c;
            CHECK(lin_expr.n_terms() == 0);
            CHECK(lin_expr.constant() == 3._a);
        }

        SECTION("with deduced type") {
            LinExpr<TestType::PlayerT> lin_expr = 3;
            CHECK(lin_expr.n_terms() == 0);
            CHECK(lin_expr.constant() == 3._a);
        }

    }

    SECTION("should make equality with (const Variable<PlayerT>&)") {

        LinExpr<TestType::PlayerT> lin_expr = x;
        CHECK(lin_expr[x] == 1._a);
        CHECK(lin_expr.constant() == 0._a);

    }

    SECTION("should make equality with (const LinExpr<PlayerT>&)") {

        SECTION("with explicit type") {
            LinExpr<TestType::PlayerT> c = 3 + 2 * x + y;
            LinExpr<TestType::PlayerT> lin_expr = (const LinExpr<TestType::PlayerT>&) c;
            CHECK(lin_expr.n_terms() == 2);
            CHECK(lin_expr[x] == 2._a);
            CHECK(lin_expr[y] == 1._a);
            CHECK(lin_expr.constant() == 3._a);
        }

        SECTION("with deduced type") {
            LinExpr<TestType::PlayerT> lin_expr = 3 + 2 * x + y;
            CHECK(lin_expr.n_terms() == 2);
            CHECK(lin_expr[x] == 2._a);
            CHECK(lin_expr[y] == 1._a);
            CHECK(lin_expr.constant() == 3._a);
        }

    }

}

#endif //OPTIMIZE_LINEXPR_TEST_CPP
