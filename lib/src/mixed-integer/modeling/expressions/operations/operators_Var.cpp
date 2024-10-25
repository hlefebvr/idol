//
// Created by henri on 26/10/22.
//
#include "idol/mixed-integer/modeling/expressions/operations/operators_Var.h"

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

QuadExpr<Var> idol::operator*(const LinExpr<Var>& t_lin_expr, const Var& t_var) {
    QuadExpr<Var> result;

    for (auto&& [var, constant] : t_lin_expr) {
        result.set(var, t_var, constant);
    }

    return result;
}

QuadExpr<Var> idol::operator*(const Var& t_var, const LinExpr<Var>& t_lin_expr) {
    return t_lin_expr * t_var;
}

QuadExpr<Var> idol::operator*(const Var& t_var1, const Var& t_var2) {
    return { t_var1, t_var2 };
}

QuadExpr<Var> idol::operator*(double t_num, QuadExpr<Var>&& t_quad_expr) {
    QuadExpr<Var> result(std::move(t_quad_expr));
    result *= t_num;
    return result;
}

QuadExpr<Var> idol::operator*(double t_num, const QuadExpr<Var>& t_quad_expr) {
    return t_num * QuadExpr<Var>(t_quad_expr);
}

QuadExpr<Var> idol::operator*(QuadExpr<Var>&& t_quad_expr, double t_num) {
    return t_num * std::move(t_quad_expr);
}

QuadExpr<Var> idol::operator*(const QuadExpr<Var>& t_quad_expr, double t_num) {
    return t_num * t_quad_expr;
}

Expr<Var> idol::operator*(double t_num, Expr<Var>&& t_expr) {
    Expr<Var> result(std::move(t_expr));
    result *= t_num;
    return result;
}

Expr<Var> idol::operator*(double t_num, const Expr<Var>& t_expr) {
    return t_num * Expr(t_expr);
}

Expr<Var> idol::operator*(Expr<Var>&& t_expr, double t_num) {
    return t_num * std::move(t_expr);
}

Expr<Var> idol::operator*(const Expr<Var>& t_expr, double t_num) {
    return Expr(t_expr) * t_num;
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

idol::Expr<idol::Var> idol::operator+(double t_term, idol::LinExpr<idol::Var> &&t_lin_expr) {
    Expr<Var> result(std::move(t_lin_expr));
    result.constant() += t_term;
    return result;
}

idol::Expr<idol::Var> idol::operator+(idol::LinExpr<idol::Var> &&t_lin_expr, double t_term) {
    Expr<Var> result(std::move(t_lin_expr));
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

idol::Expr<idol::Var> idol::operator+(double t_term, idol::QuadExpr<idol::Var> &&t_quad_expr) {
    Expr<Var> result(std::move(t_quad_expr));
    result.constant() += t_term;
    return result;
}

idol::Expr<idol::Var> idol::operator+(idol::QuadExpr<idol::Var> &&t_quad_expr, double t_term) {
    Expr<Var> result(std::move(t_quad_expr));
    result.constant() += t_term;
    return result;
}

QuadExpr<Var> idol::operator+(QuadExpr<Var>&& t_a, const QuadExpr<Var>& t_b) {
    QuadExpr<Var> result(std::move(t_a));
    result += t_b;
    return result;
}

QuadExpr<Var> idol::operator+(const QuadExpr<Var>& t_a, QuadExpr<Var>&& t_b) {
    return std::move(t_b) + t_a;
}

QuadExpr<Var> idol::operator+(QuadExpr<Var>&& t_a, QuadExpr<Var>&& t_b) {
    if (t_a.size() < t_b.size()) {
        return std::move(t_b) + t_a;
    }
    return std::move(t_a) + t_b;
}

QuadExpr<Var> idol::operator+(const QuadExpr<Var>& t_a, const QuadExpr<Var>& t_b) {
    if (t_a.size() < t_b.size()) {
        return QuadExpr<Var>(t_b) + t_a;
    }
    return QuadExpr<Var>(t_a) + t_b;
}

Expr<Var> idol::operator+(const Expr<Var>& t_a, Expr<Var>&& t_b) {
    Expr<Var> result(std::move(t_b));
    result.linear() += t_a.linear();
    result.quadratic() += t_a.quadratic();
    result.constant() += t_a.constant();
    return result;
}

Expr<Var> idol::operator+(Expr<Var>&& t_a, const Expr<Var>& t_b) {
    return t_b + std::move(t_a);
}

Expr<Var> idol::operator+(Expr<Var>&& t_a, Expr<Var>&& t_b) {
    return std::move(t_a) + t_b;
}

Expr<Var> idol::operator+(const Expr<Var>& t_a, const Expr<Var>& t_b) {
    return t_a + Expr<Var>(t_b);
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

QuadExpr<Var> idol::operator-(QuadExpr<Var>&& t_a, const QuadExpr<Var>& t_b) {
    QuadExpr<Var> result(std::move(t_a));
    result -= t_b;
    return result;
}

QuadExpr<Var> idol::operator-(const QuadExpr<Var>& t_a, const QuadExpr<Var>& t_b) {
    return QuadExpr<Var>(t_a) - t_b;
}

Expr<Var> idol::operator-(Expr<Var>&& t_a, const Expr<Var>& t_b) {
    Expr<Var> result(std::move(t_a));
    result -= t_b;
    return result;
}

Expr<Var> idol::operator-(const Expr<Var>& t_a, const Expr<Var>& t_b) {
    Expr<Var> result(t_a);
    result -= t_b;
    return result;
}