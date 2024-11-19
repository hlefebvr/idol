//
// Created by henri on 26/10/22.
//
#include "idol/mixed-integer/modeling/expressions/operations/operators_Var.h"
#include "idol/mixed-integer/modeling/variables/Var.h"

using namespace idol;

/* PRODUCT */

LinExpr<Var> idol::operator*(double t_num, const Var& t_var) {
    return { t_num, t_var };
}

LinExpr<Var> idol::operator*(const Var& t_var, double t_num) {
    return { t_num, t_var };
}

LinExpr<Var> idol::operator*(double t_num, LinExpr<Var>&& t_lin_expr) {
    LinExpr<Var> result(std::move(t_lin_expr));
    result *= t_num;
    return result;
}

LinExpr<Var> idol::operator*(LinExpr<Var>&& t_lin_expr, double t_num) {
    return t_num * std::move(t_lin_expr);
}

LinExpr<Var> idol::operator*(double t_num, const LinExpr<Var>& t_lin_expr) {
    return t_num * LinExpr<Var>(t_lin_expr);
}

LinExpr<Var> idol::operator*(const LinExpr<Var>& t_lin_expr, double t_num) {
    return t_num * LinExpr<Var>(t_lin_expr);
}

AffExpr<Var> idol::operator*(double t_num, AffExpr<Var>&& t_expr) {
    AffExpr<Var> result(std::move(t_expr));
    result *= t_num;
    return result;
}

AffExpr<Var> idol::operator*(double t_num, const AffExpr<Var>& t_expr) {
    return t_num * AffExpr(t_expr);
}

AffExpr<Var> idol::operator*(AffExpr<Var>&& t_expr, double t_num) {
    return t_num * std::move(t_expr);
}

AffExpr<Var> idol::operator*(const AffExpr<Var>& t_expr, double t_num) {
    return AffExpr(t_expr) * t_num;
}

/* ADDITION */

LinExpr<Var> idol::operator+(LinExpr<Var>&& t_lin_expr) {
    return std::move(t_lin_expr);
}

LinExpr<Var> idol::operator+(const LinExpr<Var>& t_lin_expr) {
    return t_lin_expr;
}

LinExpr<Var> idol::operator+(const Var& t_a, const Var& t_b) {
    LinExpr<Var> result(t_a);
    result += LinExpr<Var>(t_b);
    return result;
}

idol::AffExpr<idol::Var> idol::operator+(double t_term, idol::LinExpr<idol::Var> &&t_lin_expr) {
    AffExpr<Var> result(std::move(t_lin_expr));
    result.constant() += t_term;
    return result;
}

idol::AffExpr<idol::Var> idol::operator+(idol::LinExpr<idol::Var> &&t_lin_expr, double t_term) {
    AffExpr<Var> result(std::move(t_lin_expr));
    result.constant() += t_term;
    return result;
}

LinExpr<Var> idol::operator+(LinExpr<Var>&& t_lin_expr, const Var& t_var) {
    LinExpr<Var> result(std::move(t_lin_expr));
    result += LinExpr<Var>(t_var);
    return result;
}

LinExpr<Var> idol::operator+(const Var& t_var, LinExpr<Var>&& t_lin_expr) {
    return std::move(t_lin_expr) + t_var;
}

LinExpr<Var> idol::operator+(const LinExpr<Var>& t_lin_expr, const Var& t_var) {
    return LinExpr<Var>(t_lin_expr) + t_var;
}

LinExpr<Var> idol::operator+(const Var& t_var, const LinExpr<Var>& t_lin_expr) {
    return LinExpr<Var>(t_lin_expr) + t_var;
}

LinExpr<Var> idol::operator+(LinExpr<Var>&& t_a, const LinExpr<Var>& t_b) {
    LinExpr<Var> result(std::move(t_a));
    result += t_b;
    return result;
}

LinExpr<Var> idol::operator+(const LinExpr<Var>& t_a, LinExpr<Var>&& t_b) {
    return std::move(t_b) + t_a;
}

LinExpr<Var> idol::operator+(LinExpr<Var>&& t_a, LinExpr<Var>&& t_b) {
    return std::move(t_a) + t_b;
}

LinExpr<Var> idol::operator+(const LinExpr<Var>& t_a, const LinExpr<Var>& t_b) {
    return LinExpr<Var>(t_a) + t_b;
}

AffExpr<Var> idol::operator+(const AffExpr<Var>& t_a, AffExpr<Var>&& t_b) {
    AffExpr<Var> result(std::move(t_b));
    result.linear() += t_a.linear();
    result.constant() += t_a.constant();
    return result;
}

AffExpr<Var> idol::operator+(AffExpr<Var>&& t_a, const AffExpr<Var>& t_b) {
    return t_b + std::move(t_a);
}

AffExpr<Var> idol::operator+(AffExpr<Var>&& t_a, AffExpr<Var>&& t_b) {
    return std::move(t_a) + t_b;
}

AffExpr<Var> idol::operator+(const AffExpr<Var>& t_a, const AffExpr<Var>& t_b) {
    return t_a + AffExpr<Var>(t_b);
}

/* DIFFERENCE */

LinExpr<Var> idol::operator-(const LinExpr<Var>& t_lin_expr) {
    return -1 * t_lin_expr;
}

LinExpr<Var> idol::operator-(const Var& t_a, const Var& t_b) {
    LinExpr<Var> result(t_a);
    result -= LinExpr<Var>(t_b);
    return result;
}

LinExpr<Var> idol::operator-(LinExpr<Var>&& t_lin_expr, const Var& t_var) {
    LinExpr<Var> result(std::move(t_lin_expr));
    result -= LinExpr<Var>(t_var);
    return result;
}

LinExpr<Var> idol::operator-(const Var& t_var, const LinExpr<Var>& t_lin_expr) {
    LinExpr<Var> result(t_var);
    result -= t_lin_expr;
    return result;
}

LinExpr<Var> idol::operator-(LinExpr<Var>&& t_a, const LinExpr<Var>& t_b) {
    LinExpr<Var> result(std::move(t_a));
    result -= t_b;
    return result;
}

LinExpr<Var> idol::operator-(const LinExpr<Var>& t_a, const LinExpr<Var>& t_b) {
    return LinExpr<Var>(t_a) - t_b;
}

AffExpr<Var> idol::operator-(AffExpr<Var>&& t_a, const AffExpr<Var>& t_b) {
    AffExpr<Var> result(std::move(t_a));
    result -= t_b;
    return result;
}

AffExpr<Var> idol::operator-(const AffExpr<Var>& t_a, const AffExpr<Var>& t_b) {
    AffExpr<Var> result(t_a);
    result -= t_b;
    return result;
}