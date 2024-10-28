//
// Created by henri on 26/10/22.
//

#include "idol/mixed-integer/modeling/expressions/operations/operators_Ctr.h"

using namespace idol;

/* PRODUCT */

LinExpr<Ctr> idol::operator*(double t_num, const Ctr& t_ctr) {
    return { t_num, t_ctr };
}

LinExpr<Ctr> idol::operator*(const Ctr& t_ctr, double t_num) {
    return { t_num, t_ctr };
}

LinExpr<Ctr> idol::operator*(double t_num, LinExpr<Ctr>&& t_lin_expr) {
    LinExpr<Ctr> result(std::move(t_lin_expr));
    result *= t_num;
    return result;
}

LinExpr<Ctr> idol::operator*(LinExpr<Ctr>&& t_lin_expr, double t_num) {
    return t_num * std::move(t_lin_expr);
}

LinExpr<Ctr> idol::operator*(double t_num, const LinExpr<Ctr>& t_lin_expr) {
    return t_num * LinExpr<Ctr>(t_lin_expr);
}

LinExpr<Ctr> idol::operator*(const LinExpr<Ctr>& t_lin_expr, double t_num) {
    return t_num * LinExpr<Ctr>(t_lin_expr);
}

/* ADDITION */

LinExpr<Ctr> idol::operator+(LinExpr<Ctr>&& t_lin_expr) {
    return std::move(t_lin_expr);
}

LinExpr<Ctr> idol::operator+(const LinExpr<Ctr>& t_lin_expr) {
    return t_lin_expr;
}

LinExpr<Ctr> idol::operator+(const Ctr& t_a, const Ctr& t_b) {
    LinExpr<Ctr> result(t_a);
    result += LinExpr<Ctr>(t_b);
    return result;
}

LinExpr<Ctr> idol::operator+(LinExpr<Ctr>&& t_lin_expr, const Ctr& t_ctr) {
    LinExpr<Ctr> result(std::move(t_lin_expr));
    result += LinExpr<Ctr>(t_ctr);
    return result;
}

LinExpr<Ctr> idol::operator+(const Ctr& t_ctr, LinExpr<Ctr>&& t_lin_expr) {
    return std::move(t_lin_expr) + t_ctr;
}

LinExpr<Ctr> idol::operator+(const LinExpr<Ctr>& t_lin_expr, const Ctr& t_ctr) {
    return LinExpr<Ctr>(t_lin_expr) + t_ctr;
}

LinExpr<Ctr> idol::operator+(const Ctr& t_ctr, const LinExpr<Ctr>& t_lin_expr) {
    return LinExpr<Ctr>(t_lin_expr) + t_ctr;
}

LinExpr<Ctr> idol::operator+(LinExpr<Ctr>&& t_a, const LinExpr<Ctr>& t_b) {
    LinExpr<Ctr> result(std::move(t_a));
    result += t_b;
    return result;
}

LinExpr<Ctr> idol::operator+(const LinExpr<Ctr>& t_a, LinExpr<Ctr>&& t_b) {
    return std::move(t_b) + t_a;
}

LinExpr<Ctr> idol::operator+(LinExpr<Ctr>&& t_a, LinExpr<Ctr>&& t_b) {
    return std::move(t_a) + t_b;
}

LinExpr<Ctr> idol::operator+(const LinExpr<Ctr>& t_a, const LinExpr<Ctr>& t_b) {
    return LinExpr<Ctr>(t_a) + t_b;
}

Expr<Ctr> idol::operator+(const Expr<Ctr>& t_a, Expr<Ctr>&& t_b) {
    Expr<Ctr> result(std::move(t_b));
    result.linear() += t_a.linear();
    result.constant() += t_a.constant();
    return result;
}

Expr<Ctr> idol::operator+(Expr<Ctr>&& t_a, const Expr<Ctr>& t_b) {
    return t_b + std::move(t_a);
}

Expr<Ctr> idol::operator+(Expr<Ctr>&& t_a, Expr<Ctr>&& t_b) {
    return t_b + std::move(t_b);
}

Expr<Ctr> idol::operator+(const Expr<Ctr>& t_a, const Expr<Ctr>& t_b) {
    return t_a + Expr<Ctr>(t_b);
}

/* DIFFERENCE */

LinExpr<Ctr> idol::operator-(const LinExpr<Ctr>& t_lin_expr) {
    return -1 * t_lin_expr;
}

LinExpr<Ctr> idol::operator-(const Ctr& t_a, const Ctr& t_b) {
    LinExpr<Ctr> result(t_a);
    result -= LinExpr<Ctr>(t_b);
    return result;
}

LinExpr<Ctr> idol::operator-(LinExpr<Ctr>&& t_lin_expr, const Ctr& t_ctr) {
    LinExpr<Ctr> result(std::move(t_lin_expr));
    result -= LinExpr<Ctr>(t_ctr);
    return result;
}

LinExpr<Ctr> idol::operator-(const Ctr& t_ctr, const LinExpr<Ctr>& t_lin_expr) {
    LinExpr<Ctr> result(t_ctr);
    result -= t_lin_expr;
    return result;
}

LinExpr<Ctr> idol::operator-(LinExpr<Ctr>&& t_a, const LinExpr<Ctr>& t_b) {
    LinExpr<Ctr> result(std::move(t_a));
    result -= t_b;
    return result;
}

LinExpr<Ctr> idol::operator-(const LinExpr<Ctr>& t_a, const LinExpr<Ctr>& t_b) {
    return LinExpr<Ctr>(t_a) - t_b;
}


Expr<Ctr> idol::operator-(Expr<Ctr>&& t_a, const Expr<Ctr>& t_b) {
    Expr<Ctr> result(std::move(t_a));
    result -= t_b;
    return result;
}

Expr<Ctr> idol::operator-(const Expr<Ctr>& t_a, const Expr<Ctr>& t_b) {
    Expr<Ctr> result(t_a);
    result -= t_b;
    return result;
}