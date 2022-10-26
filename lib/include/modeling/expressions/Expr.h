//
// Created by henri on 21/10/22.
//

#ifndef IDOL_EXPR_H
#define IDOL_EXPR_H

#include "Row.h"
#include "Column.h"


template<class Key = Var>
class Expr {
    LinExpr<Key> m_linear;
    Constant m_constant;
public:
    Expr() = default;
    Expr(double t_num) : m_constant(t_num) {} // NOLINT(google-explicit-constructor)
    Expr(const Param& t_param) : m_constant(t_param) {} // NOLINT(google-explicit-constructor)
    Expr(const Key& t_var) : m_linear(t_var) {} // NOLINT(google-explicit-constructor)
    Expr(LinExpr<Key>&& t_expr) : m_linear(std::move(t_expr)) {} // NOLINT(google-explicit-constructor)
    Expr(const LinExpr<Key>& t_expr) : m_linear(t_expr) {} // NOLINT(google-explicit-constructor)
    Expr(Constant&& t_expr) : m_constant(std::move(t_expr)) {} // NOLINT(google-explicit-constructor)
    Expr(const Constant& t_expr) : m_constant(t_expr) {} // NOLINT(google-explicit-constructor)

    Expr(const Expr&) = default;
    Expr(Expr&&) noexcept = default;

    Expr& operator=(const Expr&) = default;
    Expr& operator=(Expr&&) noexcept = default;

    Expr<Key>& operator+=(const Expr<Key>& t_rhs);
    Expr<Key>& operator-=(const Expr<Key>& t_rhs);
    Expr<Key>& operator*=(double t_rhs);

    LinExpr<Key>& linear() { return m_linear; }
    const LinExpr<Key>& linear() const { return m_linear; }

    Constant& constant() { return m_constant; }
    const Constant& constant() const { return m_constant; }
};

template<class Key>
Expr<Key> &Expr<Key>::operator+=(const Expr<Key> &t_rhs) {
    m_linear += t_rhs.m_linear;
    m_constant += t_rhs.m_constant;
    return *this;
}

template<class Key>
Expr<Key> &Expr<Key>::operator-=(const Expr<Key> &t_rhs) {
    m_linear -= t_rhs.m_linear;
    m_constant -= t_rhs.m_constant;
    return *this;
}

template<class Key>
Expr<Key> &Expr<Key>::operator*=(double t_rhs) {
    m_linear *= t_rhs;
    m_constant *= t_rhs;
    return *this;
}

template<class Key>
std::ostream &operator<<(std::ostream& t_os, const Expr<Key>& t_expr) {
    if (t_expr.constant().is_zero()) {
        return t_os << t_expr.linear();
    }

    if (t_expr.linear().empty()) {
        return t_os << t_expr.constant();
    }

    return t_os << t_expr.constant() << " + " << t_expr.linear();
}

#endif //IDOL_EXPR_H
