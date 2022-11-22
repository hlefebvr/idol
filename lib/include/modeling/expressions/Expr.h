//
// Created by henri on 21/10/22.
//

#ifndef IDOL_EXPR_H
#define IDOL_EXPR_H

#include "LinExpr.h"
#include "QuadExpr.h"

template<class Key1 = Var, class Key2 = Key1>
class Expr {
    friend class Matrix;
    LinExpr<Key1> m_linear;
    QuadExpr<Key1, Key2> m_quadratic;
    std::unique_ptr<AbstractMatrixCoefficient> m_constant;
public:
    Expr();
    Expr(double t_num) : m_constant(std::make_unique<MatrixCoefficient>(t_num)) {} // NOLINT(google-explicit-constructor)
    Expr(const Param& t_param) : m_constant(std::make_unique<MatrixCoefficient>(t_param)) {} // NOLINT(google-explicit-constructor)
    Expr(const Key1& t_var) : m_linear(t_var), m_constant(std::make_unique<MatrixCoefficient>(0.)) {} // NOLINT(google-explicit-constructor)
    Expr(LinExpr<Key1>&& t_expr) : m_linear(std::move(t_expr)), m_constant(std::make_unique<MatrixCoefficient>(0.)) {} // NOLINT(google-explicit-constructor)
    Expr(const LinExpr<Key1>& t_expr) : m_linear(t_expr), m_constant(std::make_unique<MatrixCoefficient>(0.)) {} // NOLINT(google-explicit-constructor)
    Expr(const QuadExpr<Key1>& t_expr) : m_quadratic(t_expr), m_constant(std::make_unique<MatrixCoefficient>(0.)) {} // NOLINT(google-explicit-constructor)
    Expr(Constant&& t_expr) : m_constant(std::make_unique<MatrixCoefficient>(std::move(t_expr))) {} // NOLINT(google-explicit-constructor)
    Expr(const Constant& t_expr) : m_constant(std::make_unique<MatrixCoefficient>(t_expr)) {} // NOLINT(google-explicit-constructor)
    Expr(LinExpr<Key1> t_lin_expr, QuadExpr<Key1, Key2> t_quad_expr, Constant t_constant) : m_linear(std::move(t_lin_expr)), m_quadratic(std::move(t_quad_expr)), m_constant(std::make_unique<MatrixCoefficient>(std::move(t_constant))) {}

    Expr(const Expr& t_src);
    Expr(Expr&&) noexcept = default;

    Expr& operator=(const Expr& t_rhs);
    Expr& operator=(Expr&&) noexcept = default;

    Expr<Key1, Key2>& operator+=(const Expr<Key1, Key2>& t_rhs);
    Expr<Key1, Key2>& operator-=(const Expr<Key1, Key2>& t_rhs);
    Expr<Key1, Key2>& operator*=(double t_rhs);

    LinExpr<Key1>& linear() { return m_linear; }
    [[nodiscard]] const LinExpr<Key1>& linear() const { return m_linear; }

    QuadExpr<Key1, Key2>& quadratic() { return m_quadratic; }
    const QuadExpr<Key1, Key2>& quadratic() const { return m_quadratic; }

    Constant& constant() { return m_constant->value(); }
    [[nodiscard]] const Constant& constant() const { return m_constant->value(); }

    void set_constant(MatrixCoefficientReference&& t_coefficient) { m_constant = std::make_unique<MatrixCoefficientReference>(std::move(t_coefficient)); }
};

template<class Key1, class Key2>
Expr<Key1, Key2> &Expr<Key1, Key2>::operator+=(const Expr<Key1, Key2> &t_rhs) {
    m_linear += t_rhs.m_linear;
    m_quadratic += t_rhs.m_quadratic;
    m_constant->value() += t_rhs.m_constant->value();
    return *this;
}

template<class Key1, class Key2>
Expr<Key1, Key2> &Expr<Key1, Key2>::operator-=(const Expr<Key1, Key2> &t_rhs) {
    m_linear -= t_rhs.m_linear;
    m_quadratic -= t_rhs.m_quadratic;
    m_constant->value() -= t_rhs.m_constant->value();
    return *this;
}

template<class Key1, class Key2>
Expr<Key1, Key2> &Expr<Key1, Key2>::operator*=(double t_rhs) {
    m_linear *= t_rhs;
    m_quadratic *= t_rhs;
    m_constant->value() *= t_rhs;
    return *this;
}

template<class Key1, class Key2>
Expr<Key1, Key2>::Expr(const Expr & t_src)
    : m_linear(t_src.m_linear),
      m_quadratic(t_src.m_quadratic),
      m_constant(t_src.m_constant ? std::make_unique<MatrixCoefficient>(t_src.m_constant->value()) : std::make_unique<MatrixCoefficient>(0.)) {

}

template<class Key1, class Key2>
Expr<Key1, Key2> &Expr<Key1, Key2>::operator=(const Expr &t_rhs) {
    if (this == &t_rhs) { return *this; }
    m_linear = t_rhs.m_linear;
    m_quadratic = t_rhs.m_quadratic;
    m_constant->value() = t_rhs.m_constant->value();
    return *this;
}

template<class Key1, class Key2>
Expr<Key1, Key2>::Expr() : m_constant(std::make_unique<MatrixCoefficient>(0.)) {

}

template<class Key1, class Key2>
std::ostream &operator<<(std::ostream& t_os, const Expr<Key1, Key2>& t_expr) {

    if (t_expr.constant().is_zero()) {

        if (t_expr.linear().empty()) {
            return t_os << t_expr.quadratic();
        }

        t_os << t_expr.linear();

        if (!t_expr.quadratic().empty()) {
            return t_os << " + " << t_expr.quadratic();
        }
    }

    t_os << t_expr.constant();

    if (!t_expr.linear().empty()) {
        t_os << " + " << t_expr.linear();
    }

    if (!t_expr.quadratic().empty()) {
        t_os << " + " << t_expr.quadratic();
    }

    return t_os;
}

#endif //IDOL_EXPR_H
