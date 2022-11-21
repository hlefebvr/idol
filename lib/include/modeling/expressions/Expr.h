//
// Created by henri on 21/10/22.
//

#ifndef IDOL_EXPR_H
#define IDOL_EXPR_H

#include "Row.h"
#include "Column.h"
#include "QuadExpr.h"

template<class Key = Var>
class Expr {
    //friend class Matrix;
    LinExpr<Key> m_linear;
    QuadExpr<Key> m_quadratic;
    std::unique_ptr<AbstractMatrixCoefficient> m_constant;
public:
    Expr();
    Expr(double t_num) : m_constant(std::make_unique<MatrixCoefficient>(t_num)) {} // NOLINT(google-explicit-constructor)
    Expr(const Param& t_param) : m_constant(std::make_unique<MatrixCoefficient>(t_param)) {} // NOLINT(google-explicit-constructor)
    Expr(const Key& t_var) : m_linear(t_var), m_constant(std::make_unique<MatrixCoefficient>(0.)) {} // NOLINT(google-explicit-constructor)
    Expr(LinExpr<Key>&& t_expr) : m_linear(std::move(t_expr)), m_constant(std::make_unique<MatrixCoefficient>(0.)) {} // NOLINT(google-explicit-constructor)
    Expr(const LinExpr<Key>& t_expr) : m_linear(t_expr), m_constant(std::make_unique<MatrixCoefficient>(0.)) {} // NOLINT(google-explicit-constructor)
    Expr(const QuadExpr<Key>& t_expr) : m_quadratic(t_expr), m_constant(std::make_unique<MatrixCoefficient>(0.)) {} // NOLINT(google-explicit-constructor)
    Expr(Constant&& t_expr) : m_constant(std::make_unique<MatrixCoefficient>(std::move(t_expr))) {} // NOLINT(google-explicit-constructor)
    Expr(const Constant& t_expr) : m_constant(std::make_unique<MatrixCoefficient>(t_expr)) {} // NOLINT(google-explicit-constructor)

    Expr(const Expr& t_src);
    Expr(Expr&&) noexcept = default;

    Expr& operator=(const Expr& t_rhs);
    Expr& operator=(Expr&&) noexcept = default;

    Expr<Key>& operator+=(const Expr<Key>& t_rhs);
    Expr<Key>& operator-=(const Expr<Key>& t_rhs);
    Expr<Key>& operator*=(double t_rhs);

    LinExpr<Key>& linear() { return m_linear; }
    const LinExpr<Key>& linear() const { return m_linear; }

    QuadExpr<Key>& quadratic() { return m_quadratic; }
    const QuadExpr<Key>& quadratic() const { return m_quadratic; }

    Constant& constant() { return m_constant->value(); }
    [[nodiscard]] const Constant& constant() const { return m_constant->value(); }
};

template<class Key>
Expr<Key> &Expr<Key>::operator+=(const Expr<Key> &t_rhs) {
    m_linear += t_rhs.m_linear;
    m_quadratic += t_rhs.m_quadratic;
    m_constant->value() += t_rhs.m_constant->value();
    return *this;
}

template<class Key>
Expr<Key> &Expr<Key>::operator-=(const Expr<Key> &t_rhs) {
    m_linear -= t_rhs.m_linear;
    m_quadratic -= t_rhs.m_quadratic;
    m_constant->value() -= t_rhs.m_constant->value();
    return *this;
}

template<class Key>
Expr<Key> &Expr<Key>::operator*=(double t_rhs) {
    m_linear *= t_rhs;
    m_quadratic *= t_rhs;
    m_constant->value() *= t_rhs;
    return *this;
}

template<class Key>
Expr<Key>::Expr(const Expr & t_src)
    : m_linear(t_src.m_linear),
      m_quadratic(t_src.m_quadratic),
      m_constant(t_src.m_constant ? std::make_unique<MatrixCoefficient>(t_src.m_constant->value()) : std::make_unique<MatrixCoefficient>(0.)) {

}

template<class Key>
Expr<Key> &Expr<Key>::operator=(const Expr &t_rhs) {
    if (this == &t_rhs) { return *this; }
    m_linear = t_rhs.m_linear;
    m_quadratic = t_rhs.m_quadratic;
    m_constant->value() = t_rhs.m_constant->value();
    return *this;
}

template<class Key>
Expr<Key>::Expr() : m_constant(std::make_unique<MatrixCoefficient>(0.)) {

}

template<class Key>
std::ostream &operator<<(std::ostream& t_os, const Expr<Key>& t_expr) {

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
