//
// Created by henri on 26/10/22.
//

#include "idol/mixed-integer/modeling/expressions/operations/operators_Ctr.h"
#include "idol/mixed-integer/modeling/constraints/Ctr.h"

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

AffExpr<Ctr> idol::operator+(const AffExpr<Ctr>& t_a, AffExpr<Ctr>&& t_b) {
    AffExpr<Ctr> result(std::move(t_b));
    result.linear() += t_a.linear();
    result.constant() += t_a.constant();
    return result;
}

AffExpr<Ctr> idol::operator+(AffExpr<Ctr>&& t_a, const AffExpr<Ctr>& t_b) {
    return t_b + std::move(t_a);
}

AffExpr<Ctr> idol::operator+(AffExpr<Ctr>&& t_a, AffExpr<Ctr>&& t_b) {
    return t_b + std::move(t_b);
}

AffExpr<Ctr> idol::operator+(const AffExpr<Ctr>& t_a, const AffExpr<Ctr>& t_b) {
    return t_a + AffExpr<Ctr>(t_b);
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


AffExpr<Ctr> idol::operator-(AffExpr<Ctr>&& t_a, const AffExpr<Ctr>& t_b) {
    AffExpr<Ctr> result(std::move(t_a));
    result -= t_b;
    return result;
}

AffExpr<Ctr> idol::operator-(const AffExpr<Ctr>& t_a, const AffExpr<Ctr>& t_b) {
    AffExpr<Ctr> result(t_a);
    result -= t_b;
    return result;
}