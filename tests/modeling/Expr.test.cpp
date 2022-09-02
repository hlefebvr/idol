//
// Created by henri on 02/09/22.
//

#include <iostream>
#include "../test_utils.h"

TEMPLATE_LIST_TEST_CASE("Expr", "[expressions][modeling]", for_each_player) {

    Env env;
    Model<TestType::PlayerT> model(env);
    auto x = model.add_variable(0., Inf, Continuous, "x");
    auto y = model.add_variable(0., Inf, Continuous, "y");
    auto xi_1 = model.add_parameter(0., Inf, Continuous, "xi_1");
    auto xi_2 = model.add_parameter(0., Inf, Continuous, "xi_2");

    SECTION("should construct with (double)") {
        Expr<TestType::PlayerT> expr(3);
        CHECK(expr.n_terms() == 0);
        CHECK(expr.constant().n_terms() == 0);
        CHECK(expr.constant().constant() == 3._a);
    }

    SECTION("should construct with (Variable<PlayerT>)") {
        Expr<TestType::PlayerT> expr(x);
        CHECK(expr.n_terms() == 1);
        CHECK(expr[x].constant() == 1.);
        CHECK(expr.constant().n_terms() == 0);
        CHECK(expr.constant().constant() == 0._a);
    }

    SECTION("should construct with (Variable<opp_player_v<PlayerT>>)") {
        Expr<TestType::PlayerT> expr(xi_1);
        CHECK(expr.n_terms() == 0);
        CHECK(expr.constant().n_terms() == 1);
        CHECK(expr.constant()[xi_1] == 1._a);
        CHECK(expr.constant().constant() == 0._a);
    }

    SECTION("should construct with (LinExpr<PlayerT>)") {

        SECTION("with explicit type") {
            LinExpr<TestType::PlayerT> lin_expr = 3 + 2 * x;
            Expr<TestType::PlayerT> expr(lin_expr);
            CHECK(expr.n_terms() == 1);
            CHECK(expr[x].constant() == 2._a);
            CHECK(expr.constant().constant() == 3._a);
        }

        SECTION("with deduced type") {
            Expr<TestType::PlayerT> expr(3 + 2 * x);
            CHECK(expr.n_terms() == 1);
            CHECK(expr[x].constant() == 2._a);
            CHECK(expr.constant().constant() == 3._a);
        }

    }

    SECTION("should construct with (LinExpr<opp_player_v<PlayerT>>)") {

        SECTION("with explicit type") {
            LinExpr<opp_player_v<TestType::PlayerT>> lin_expr = 3 + 2 * xi_1;
            Expr<TestType::PlayerT> expr(lin_expr);
            CHECK(expr.n_terms() == 0);
            CHECK(expr.constant().n_terms() == 1);
            CHECK(expr.constant()[xi_1] == 2._a);
            CHECK(expr.constant().constant() == 3.);
        }

        SECTION("with deduced type") {
            Expr<TestType::PlayerT> expr(3 + 2 * xi_1);
            CHECK(expr.n_terms() == 0);
            CHECK(expr.constant().n_terms() == 1);
            CHECK(expr.constant()[xi_1] == 2._a);
            CHECK(expr.constant().constant() == 3.);
        }

    }

    SECTION("should construct with (Expr<PlayerT>)") {

        SECTION("with explicit type") {
            Expr<TestType::PlayerT> src_expr = 3 + xi_1 + (1 + 2 * xi_1) * x + y;
            Expr<TestType::PlayerT> expr(src_expr);
            CHECK(expr.n_terms() == 2);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr[y].constant() == 1._a);
            CHECK(expr.constant().n_terms() == 1);
            CHECK(expr.constant()[xi_1] == 1.);
            CHECK(expr.constant().constant() == 3.);
        }

        SECTION("with deduced type") {
            Expr<TestType::PlayerT> expr(3 + xi_1 + (1 + 2 * xi_1) * x + y);
            CHECK(expr.n_terms() == 2);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr[y].constant() == 1._a);
            CHECK(expr.constant().n_terms() == 1);
            CHECK(expr.constant()[xi_1] == 1.);
            CHECK(expr.constant().constant() == 3.);
        }

    }

    SECTION("should make equality with (double)") {

        SECTION("with explicit type") {
            double c = 3;
            Expr<TestType::PlayerT> expr = c;
            CHECK(expr.is_numerical());
            CHECK(expr.constant().constant() == 3._a);
        }

        SECTION("with deduced type") {
            Expr<TestType::PlayerT> expr = 3;
            CHECK(expr.is_numerical());
            CHECK(expr.constant().constant() == 3._a);
        }

    }

    SECTION("should make equality with (Variable<PlayerT>)") {

        Expr<TestType::PlayerT> expr = x;
        CHECK(expr.n_terms() == 1);
        CHECK(expr[x].constant() == 1._a);
        CHECK(expr.constant().is_numerical());
        CHECK(expr.constant().constant() == 0._a);

    }

    SECTION("should make equality with (Variable<opp_player_v<PlayerT>>)") {
        Expr<TestType::PlayerT> expr = xi_1;
        CHECK(expr.n_terms() == 0);
        CHECK(expr.constant()[xi_1] == 1._a);
        CHECK(expr.constant().constant() == 0._a);
    }

    SECTION("should make equality with (LinExpr<PlayerT>)") {

        SECTION("with explicit type") {
            LinExpr<TestType::PlayerT> lin_expr = 3 + 2 * x;
            Expr<TestType::PlayerT> expr = (const LinExpr<TestType::PlayerT>&) lin_expr;
            CHECK(expr.n_terms() == 1);
            CHECK(expr[x].constant() == 2._a);
            CHECK(expr.constant().constant() == 3._a);
        }

        SECTION("with deduced type") {
            Expr<TestType::PlayerT> expr = 3 + 2 * x;
            CHECK(expr.n_terms() == 1);
            CHECK(expr[x].constant() == 2._a);
            CHECK(expr.constant().constant() == 3._a);
        }

    }

    SECTION("should make equality with (LinExpr<opp_player_v<PlayerT>>)") {

        SECTION("with explicit type") {
            LinExpr<opp_player_v<TestType::PlayerT>> lin_expr = 3 + 2 * xi_1;
            Expr<TestType::PlayerT> expr = lin_expr;
            CHECK(expr.n_terms() == 0);
            CHECK(expr.constant().n_terms() == 1);
            CHECK(expr.constant()[xi_1] == 2._a);
            CHECK(expr.constant().constant() == 3.);
        }

        SECTION("with deduced type") {
            Expr<TestType::PlayerT> expr = 3 + 2 * xi_1;
            CHECK(expr.n_terms() == 0);
            CHECK(expr.constant().n_terms() == 1);
            CHECK(expr.constant()[xi_1] == 2._a);
            CHECK(expr.constant().constant() == 3.);
        }

    }

    SECTION("should make equality with (Expr<PlayerT>)") {

        SECTION("with explicit type") {
            Expr<TestType::PlayerT> src_expr = 3 + xi_1 + (1 + 2 * xi_1) * x + y;
            Expr<TestType::PlayerT> expr = src_expr;
            CHECK(expr.n_terms() == 2);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr[y].constant() == 1._a);
            CHECK(expr.constant().n_terms() == 1);
            CHECK(expr.constant()[xi_1] == 1.);
            CHECK(expr.constant().constant() == 3.);
        }

        SECTION("with deduced type") {
            auto expr = 3 + xi_1 + (1 + 2 * xi_1) * x + y;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr.n_terms() == 2);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr[y].constant() == 1._a);
            CHECK(expr.constant().n_terms() == 1);
            CHECK(expr.constant()[xi_1] == 1.);
            CHECK(expr.constant().constant() == 3.);
        }

    }

    SECTION("should make product between (Variable<opp_player_v<PlayerT>>, Variable<PlayerT>)") {

        auto expr = xi_1 * x;
        CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
        CHECK(expr.n_terms() == 1);
        CHECK(expr[x][xi_1] == 1._a);
        CHECK(expr.constant().is_numerical());

    }

    SECTION("should make sum between (Expr<PlayerT>, double)") {

        SECTION("with explicit type") {
            Expr<TestType::PlayerT> src = xi_1 * x;
            double c = 3.;
            auto expr = src + c;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr.constant().is_numerical());
            CHECK(expr.constant().constant() == 3._a);
        }

        SECTION("with deduced type") {
            Expr<TestType::PlayerT> src = xi_1 * x;
            auto expr = src + 3;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr.constant().is_numerical());
            CHECK(expr.constant().constant() == 3._a);
        }


        SECTION("in one line") {
            auto expr = xi_1 * x + 3;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr.constant().is_numerical());
            CHECK(expr.constant().constant() == 3._a);
        }


    }

    SECTION("should make sum between (double, Expr<PlayerT>)") {

        SECTION("with explicit type") {
            Expr<TestType::PlayerT> src = xi_1 * x;
            double c = 3.;
            auto expr = c + src;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr.constant().is_numerical());
            CHECK(expr.constant().constant() == 3._a);
        }

        SECTION("with deduced type") {
            Expr<TestType::PlayerT> src = xi_1 * x;
            auto expr = 3 + src;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr.constant().is_numerical());
            CHECK(expr.constant().constant() == 3._a);
        }

        SECTION("in one line") {
            auto expr = 3 + xi_1 * x;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr.constant().is_numerical());
            CHECK(expr.constant().constant() == 3._a);
        }

    }

    SECTION("should make sum between (Var<opp_player_v<PlayerT>>, Var<PlayerT>)") {

        auto expr = xi_1 + x;
        CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
        CHECK(expr[x].constant() == 1._a);
        CHECK(expr[x][xi_1] == 0._a);
        CHECK(expr.constant()[xi_1] == 1._a);

    }

    SECTION("should make sum between (LinExpr<opp_player_v<PlayerT>>, Var<PlayerT>)") {

        SECTION("with explicit type") {
            LinExpr<TestType::OppositePlayerT> c = 2 * xi_1;
            auto expr = c + x;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 0._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

        SECTION("with deduced type") {
            auto expr = 2 * xi_1 + x;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 0._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

    }

    SECTION("should make sum between (LinExpr<opp_player_v<PlayerT>>, LinExpr<PlayerT>)") {

        SECTION("with explicit type") {
            LinExpr<TestType::OppositePlayerT> a = 2 * xi_1;
            LinExpr<TestType::PlayerT> b = 2 * x;
            auto expr = a + b;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 2._a);
            CHECK(expr[x][xi_1] == 0._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

        SECTION("with deduced type for LinExpr<PlayerT>") {
            LinExpr<TestType::OppositePlayerT> a = 2 * xi_1;
            auto expr = a + 2 * x;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 2._a);
            CHECK(expr[x][xi_1] == 0._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

        SECTION("with deduced type for LinExpr<opp_player_v<PlayerT>>") {
            LinExpr<TestType::PlayerT> b = 2 * x;
            auto expr = 2 * xi_1 + b;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 2._a);
            CHECK(expr[x][xi_1] == 0._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

        SECTION("with deduced type") {
            auto expr = 2 * xi_1 + 2 * x;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 2._a);
            CHECK(expr[x][xi_1] == 0._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

    }

    SECTION("should make sum between (Expr<PlayerT>, Variable<PlayerT>)") {

        SECTION("with \"explicit\" types") {
            Expr<TestType::PlayerT> src = xi_1 * x;
            auto expr = src + x;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr[x].constant() == 1.);
            CHECK(expr.constant().is_numerical());
            CHECK(expr.constant().constant() == 0._a);
        }

        SECTION("in one line") {
            auto expr = xi_1 * x + x;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr[x].constant() == 1.);
            CHECK(expr.constant().is_numerical());
            CHECK(expr.constant().constant() == 0._a);
        }

    }

    SECTION("should make sum between (Variable<PlayerT>, Expr<PlayerT>)") {

        SECTION("with \"explicit\" types") {
            Expr<TestType::PlayerT> src = xi_1 * x;
            auto expr = x + src;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr[x].constant() == 1.);
            CHECK(expr.constant().is_numerical());
            CHECK(expr.constant().constant() == 0._a);
        }

        SECTION("in one line") {
            auto expr = x + xi_1 * x;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr[x].constant() == 1.);
            CHECK(expr.constant().is_numerical());
            CHECK(expr.constant().constant() == 0._a);
        }

    }

    SECTION("should make sum between (Expr<PlayerT>, Variable<opp_player_v<PlayerT>>)") {

        SECTION("with \"explicit\" types") {
            Expr<TestType::PlayerT> src = xi_1 * x;
            auto expr = src + xi_1;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr[x].constant() == 0.);
            CHECK(expr.constant()[xi_1] == 1.);
            CHECK(expr.constant().constant() == 0._a);
        }

        SECTION("in one line") {
            auto expr = xi_1 * x + xi_1;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr[x].constant() == 0.);
            CHECK(expr.constant()[xi_1] == 1.);
            CHECK(expr.constant().constant() == 0._a);
        }

    }

    SECTION("should make sum between (Variable<opp_player_v<PlayerT>>, Expr<PlayerT>)") {

        SECTION("with explicit type") {
            Expr<TestType::PlayerT> src = xi_1 * x;
            auto expr = xi_1 + src;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr[x].constant() == 0.);
            CHECK(expr.constant()[xi_1] == 1.);
            CHECK(expr.constant().constant() == 0._a);
        }

        SECTION("with deduced type") {
            auto expr = xi_1 + xi_1 * x;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 1.);
            CHECK(expr[x].constant() == 0.);
            CHECK(expr.constant()[xi_1] == 1.);
            CHECK(expr.constant().constant() == 0._a);
        }

    }

    SECTION("should make sum between (Expr<PlayerT>, LinExpr<PlayerT>)") {

        SECTION("with explicit type") {
            LinExpr<TestType::PlayerT> lin_expr = 2 * x + 10.;
            Expr<TestType::PlayerT> src = (1 + 2 * xi_1) * x;
            auto expr = src + lin_expr;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 3._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
        }

        SECTION("with deduced type for LinExpr<PlayerT>") {
            Expr<TestType::PlayerT> src = (1 + 2 * xi_1) * x;
            auto expr = src + 2 * x + 10.;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 3._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
        }

        SECTION("with deduced type for Expr<Player>") {
            LinExpr<TestType::PlayerT> lin_expr = 2 * x + 10.;
            auto expr = (1 + 2 * xi_1) * x + lin_expr;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 3._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
        }

        SECTION("with deduced type") {
            auto expr = (1 + 2 * xi_1) * x + 2 * x + 10.;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 3._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
        }

    }

    SECTION("should make sum between (LinExpr<PlayerT>, Expr<PlayerT>)") {

        SECTION("with explicit type") {
            LinExpr<TestType::PlayerT> lin_expr = 2 * x + 10.;
            Expr<TestType::PlayerT> src = (1 + 2 * xi_1) * x;
            auto expr = lin_expr + src;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 3._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
        }

        SECTION("with deduced type for LinExpr<PlayerT>") {
            Expr<TestType::PlayerT> src = (1 + 2 * xi_1) * x;
            auto expr = 2 * x + 10. + src;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 3._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
        }

        SECTION("with deduced type for Expr<PlayerT>") {
            LinExpr<TestType::PlayerT> lin_expr = 2 * x + 10.;
            auto expr = lin_expr + (1 + 2 * xi_1) * x;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 3._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
        }

        SECTION("with deduced type") {
            auto expr = 2 * x + 10. + (1 + 2 * xi_1) * x;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 3._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
        }

    }

    SECTION("should make sum between (Expr<PlayerT>, LinExpr<opp_player_v<PlayerT>>)") {

        SECTION("with explicit type") {
            LinExpr<opp_player_v<TestType::PlayerT>> lin_expr = 2 * xi_1 + 10.;
            Expr<TestType::PlayerT> src = (1 + 2 * xi_1) * x;
            auto expr = src + lin_expr;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

        SECTION("with deduced type for LinExpr<PlayerT>") {
            Expr<TestType::PlayerT> src = (1 + 2 * xi_1) * x;
            auto expr = src + 2 * xi_1 + 10.;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

        SECTION("with deduced type for Expr<Player>") {
            LinExpr<opp_player_v<TestType::PlayerT>> lin_expr = 2 * xi_1 + 10.;
            auto expr = (1 + 2 * xi_1) * x + lin_expr;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

        SECTION("with deduced type") {
            auto expr = (1 + 2 * xi_1) * x + 2 * xi_1 + 10.;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

    }

    SECTION("should make sum between (LinExpr<opp_player_v<PlayerT>>, Expr<PlayerT>)") {

        SECTION("with explicit type") {
            LinExpr<opp_player_v<TestType::PlayerT>> lin_expr = 2 * xi_1 + 10.;
            Expr<TestType::PlayerT> src = (1 + 2 * xi_1) * x;
            auto expr = lin_expr + src;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

        SECTION("with deduced type for LinExpr<PlayerT>") {
            Expr<TestType::PlayerT> src = (1 + 2 * xi_1) * x;
            auto expr = 2 * xi_1 + 10. + src;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

        SECTION("with deduced type for Expr<Player>") {
            LinExpr<opp_player_v<TestType::PlayerT>> lin_expr = 2 * xi_1 + 10.;
            auto expr = lin_expr + (1 + 2 * xi_1) * x;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

        SECTION("with deduced type") {
            auto expr = 2 * xi_1 + 10. + (1 + 2 * xi_1) * x ;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[x][xi_1] == 2._a);
            CHECK(expr.constant().constant() == 10._a);
            CHECK(expr.constant()[xi_1] == 2._a);
        }

    }

    SECTION("should make sum between (Expr<PlayerT>, Expr<PlayerT>)") {

        SECTION("with explicit type") {
            Expr<TestType::PlayerT> e1 = (1 + 3 * xi_1) * x + xi_2 * y + 2;
            Expr<TestType::PlayerT> e2 = (xi_1 + xi_2) * x + 2 + y + xi_2 + 4 * xi_1;
            auto expr = e1 + e2;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 4._a);
            CHECK(expr[x][xi_2] == 1._a);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[y][xi_1] == 0._a);
            CHECK(expr[y][xi_2] == 1._a);
            CHECK(expr[y].constant() == 1._a);
            CHECK(expr.constant()[xi_1] == 4._a);
            CHECK(expr.constant()[xi_2] == 1._a);
            CHECK(expr.constant().constant() == 4._a);
        }

        SECTION("with deduced type for first Expr<PlayerT>") {
            Expr<TestType::PlayerT> e2 = (xi_1 + xi_2) * x + 2 + y + xi_2 + 4 * xi_1;
            auto expr = (1 + 3 * xi_1) * x + xi_2 * y + 2 + e2;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 4._a);
            CHECK(expr[x][xi_2] == 1._a);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[y][xi_1] == 0._a);
            CHECK(expr[y][xi_2] == 1._a);
            CHECK(expr[y].constant() == 1._a);
            CHECK(expr.constant()[xi_1] == 4._a);
            CHECK(expr.constant()[xi_2] == 1._a);
            CHECK(expr.constant().constant() == 4._a);
        }

        SECTION("with deduced type for second Expr<PlayerT>") {
            Expr<TestType::PlayerT> e1 = (1 + 3 * xi_1) * x + xi_2 * y + 2;
            auto expr = e1 + (xi_1 + xi_2) * x + 2 + y + xi_2 + 4 * xi_1;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 4._a);
            CHECK(expr[x][xi_2] == 1._a);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[y][xi_1] == 0._a);
            CHECK(expr[y][xi_2] == 1._a);
            CHECK(expr[y].constant() == 1._a);
            CHECK(expr.constant()[xi_1] == 4._a);
            CHECK(expr.constant()[xi_2] == 1._a);
            CHECK(expr.constant().constant() == 4._a);
        }

        SECTION("with deduced type") {
            auto expr = (1 + 3 * xi_1) * x + xi_2 * y + 2 + (xi_1 + xi_2) * x + 2 + y + xi_2 + 4 * xi_1;
            CHECK(std::is_same_v<Expr<TestType::PlayerT>, decltype(expr)>);
            CHECK(expr[x][xi_1] == 4._a);
            CHECK(expr[x][xi_2] == 1._a);
            CHECK(expr[x].constant() == 1._a);
            CHECK(expr[y][xi_1] == 0._a);
            CHECK(expr[y][xi_2] == 1._a);
            CHECK(expr[y].constant() == 1._a);
            CHECK(expr.constant()[xi_1] == 4._a);
            CHECK(expr.constant()[xi_2] == 1._a);
            CHECK(expr.constant().constant() == 4._a);
        }

    }

}