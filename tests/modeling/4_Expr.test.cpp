//
// Created by henri on 08/09/22.
//
#include "../test_utils.h"

TEST_CASE("04. Expr", "[expressions][modeling]") {

    Model model;

    auto x = model.add_var(0., 1., Continuous, 0.);
    auto y = model.add_var(0., 1., Continuous, 0.);

    SECTION("Type deduction for operator+") {

        CHECK(std::is_same_v<decltype(-x), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(x + y), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(2 * x + y), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(x + 2 * y), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(2 * x + 2 * y), LinExpr<Var>>);

        CHECK(std::is_same_v<decltype(-!x), Constant>);
        CHECK(std::is_same_v<decltype(!x + !y), Constant>);
        CHECK(std::is_same_v<decltype(!x + 1), Constant>);
        CHECK(std::is_same_v<decltype(1 + !x), Constant>);
        CHECK(std::is_same_v<decltype(2 * !x + !y), Constant>);
        CHECK(std::is_same_v<decltype(!x + 2 * !y), Constant>);
        CHECK(std::is_same_v<decltype(2 * !x + 2 * !y), Constant>);
        CHECK(std::is_same_v<decltype(1 + 2 * !x + !y), Constant>);
        CHECK(std::is_same_v<decltype(1 + !x + 2 * !y), Constant>);
        CHECK(std::is_same_v<decltype(1 + 2 * !x + 2 * !y), Constant>);
        CHECK(std::is_same_v<decltype(2 * !x + !y + 1), Constant>);
        CHECK(std::is_same_v<decltype(!x + 2 * !y + 1), Constant>);
        CHECK(std::is_same_v<decltype(2 * !x + 2 * !y + 1), Constant>);

        Constant con1, con2;

        CHECK(std::is_same_v<decltype(con1 + 1), Constant>);
        CHECK(std::is_same_v<decltype(1 + con1), Constant>);
        CHECK(std::is_same_v<decltype(con1 + !x), Constant>);
        CHECK(std::is_same_v<decltype(!x + con1), Constant>);
        CHECK(std::is_same_v<decltype(con1 + 2 * !x), Constant>);
        CHECK(std::is_same_v<decltype(2 * !x + con1), Constant>);
        CHECK(std::is_same_v<decltype(con1 + con2), Constant>);

        LinExpr<Var> lin1, lin2;
        CHECK(std::is_same_v<decltype(lin1 + x), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(x + lin1), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(lin1 + 2 * x), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(2 * x + lin1), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(lin1 + lin2), LinExpr<Var>>);

        Expr<Var> expr1, expr2;
        CHECK(std::is_same_v<decltype(2 * expr1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 * 2), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 + 1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(1 + expr1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 + x), Expr<Var>>);
        CHECK(std::is_same_v<decltype(x + expr1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 + !x), Expr<Var>>);
        CHECK(std::is_same_v<decltype(!x + expr1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 + 2 * x), Expr<Var>>);
        CHECK(std::is_same_v<decltype(2 * x + expr1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 + 2 * !x), Expr<Var>>);
        CHECK(std::is_same_v<decltype(2 * !x + expr1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 + expr2), Expr<Var>>);

        CHECK(std::is_same_v<decltype(x + !x + 2), Expr<Var>>);
        CHECK(std::is_same_v<decltype(x + 2 * !x + 2), Expr<Var>>);
        CHECK(std::is_same_v<decltype(!x + 2 + x), Expr<Var>>);
        CHECK(std::is_same_v<decltype(!x + x), Expr<Var>>);
        CHECK(std::is_same_v<decltype(2 * (!x + x)), Expr<Var>>);

    }

    SECTION("Type deduction for operator-") {

        CHECK(std::is_same_v<decltype(+x), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(x - y), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(2 * x - y), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(x - 2 * y), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(2 * x - 2 * y), LinExpr<Var>>);

        CHECK(std::is_same_v<decltype(+!x), Constant>);
        CHECK(std::is_same_v<decltype(!x - !y), Constant>);
        CHECK(std::is_same_v<decltype(!x - 1), Constant>);
        CHECK(std::is_same_v<decltype(1 - !x), Constant>);
        CHECK(std::is_same_v<decltype(2 * !x - !y), Constant>);
        CHECK(std::is_same_v<decltype(!x - 2 * !y), Constant>);
        CHECK(std::is_same_v<decltype(2 * !x - 2 * !y), Constant>);
        CHECK(std::is_same_v<decltype(1 - 2 * !x - !y), Constant>);
        CHECK(std::is_same_v<decltype(1 - !x - 2 * !y), Constant>);
        CHECK(std::is_same_v<decltype(1 - 2 * !x - 2 * !y), Constant>);
        CHECK(std::is_same_v<decltype(2 * !x - !y - 1), Constant>);
        CHECK(std::is_same_v<decltype(!x - 2 * !y - 1), Constant>);
        CHECK(std::is_same_v<decltype(2 * !x - 2 * !y - 1), Constant>);

        Constant con1, con2;

        CHECK(std::is_same_v<decltype(con1 - 1), Constant>);
        CHECK(std::is_same_v<decltype(1 - con1), Constant>);
        CHECK(std::is_same_v<decltype(con1 - !x), Constant>);
        CHECK(std::is_same_v<decltype(!x - con1), Constant>);
        CHECK(std::is_same_v<decltype(con1 - 2 * !x), Constant>);
        CHECK(std::is_same_v<decltype(2 * !x - con1), Constant>);
        CHECK(std::is_same_v<decltype(con1 - con2), Constant>);

        LinExpr<Var> lin1, lin2;
        CHECK(std::is_same_v<decltype(lin1 - x), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(x - lin1), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(lin1 - 2 * x), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(2 * x - lin1), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(lin1 - lin2), LinExpr<Var>>);

        Expr<Var> expr1, expr2;
        CHECK(std::is_same_v<decltype(2 * expr1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 * 2), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 - 1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(1 - expr1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 - x), Expr<Var>>);
        CHECK(std::is_same_v<decltype(x - expr1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 - !x), Expr<Var>>);
        CHECK(std::is_same_v<decltype(!x - expr1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 - 2 * x), Expr<Var>>);
        CHECK(std::is_same_v<decltype(2 * x - expr1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 - 2 * !x), Expr<Var>>);
        CHECK(std::is_same_v<decltype(2 * !x - expr1), Expr<Var>>);
        CHECK(std::is_same_v<decltype(expr1 - expr2), Expr<Var>>);

        CHECK(std::is_same_v<decltype(x - !x - 2), Expr<Var>>);
        CHECK(std::is_same_v<decltype(x - 2 * !x - 2), Expr<Var>>);
        CHECK(std::is_same_v<decltype(!x - 2 - x), Expr<Var>>);
        CHECK(std::is_same_v<decltype(!x - x), Expr<Var>>);
        CHECK(std::is_same_v<decltype(2 * (!x - x)), Expr<Var>>);

    }

}