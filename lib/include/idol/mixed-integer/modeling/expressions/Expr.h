//
// Created by henri on 21/10/22.
//

#ifndef IDOL_EXPR_H
#define IDOL_EXPR_H

#include "LinExpr.h"
#include "idol/general/numericals.h"

namespace idol {
    class Var;

    template<class Key1, class Key2, class ValueT>
    class Expr;
}

template<class Key1 = idol::Var, class Key2 = Key1, class ValueT = double>
class idol::Expr {
    LinExpr<Key1, ValueT> m_linear;
    double m_constant = 0.;
public:
    Expr();
    Expr(double t_constant); // NOLINT(google-explicit-constructor)
    Expr(const Key1& t_var); // NOLINT(google-explicit-constructor)
    Expr(LinExpr<Key1>&& t_expr); // NOLINT(google-explicit-constructor)
    Expr(const LinExpr<Key1>& t_expr); // NOLINT(google-explicit-constructor)

    virtual ~Expr() = default;

    Expr(const Expr& t_src);
    Expr(Expr&&) noexcept = default;

    Expr& operator=(const Expr& t_rhs);
    Expr& operator=(Expr&&) noexcept = default;

    Expr& operator+=(const Expr& t_rhs);
    Expr& operator-=(const Expr& t_rhs);
    Expr& operator*=(double t_rhs);
    Expr& operator/=(double t_rhs);
    Expr operator-() const;

    LinExpr<Key1, ValueT>& linear() { return m_linear; }
    [[nodiscard]] const LinExpr<Key1, ValueT>& linear() const { return m_linear; }

    double& constant() { return m_constant; }

    [[nodiscard]] double constant() const { return m_constant; }

    [[nodiscard]] bool is_zero(double t_tolerance) const { return std::abs(constant()) < t_tolerance && linear().is_zero(t_tolerance); }

    void clear() {
        constant() = 0;
        m_linear.clear();
    }
};

template<class Key1, class Key2, class ValueT>
idol::Expr<Key1, Key2, ValueT> idol::Expr<Key1, Key2, ValueT>::operator-() const {
    auto result = *this;
    result.constant() = -result.constant();
    result.linear() = -result.linear();
    return result;
}

template<class Key1, class Key2, class ValueT>
idol::Expr<Key1, Key2, ValueT>::Expr() {

}

template<class Key1, class Key2, class ValueT>
idol::Expr<Key1, Key2, ValueT>::Expr(double t_constant) : m_constant(t_constant) {

}

template<class Key1, class Key2, class ValueT>
idol::Expr<Key1, Key2, ValueT>::Expr(const Key1 &t_var) : m_linear(t_var) {

}

template<class Key1, class Key2, class ValueT>
idol::Expr<Key1, Key2, ValueT>::Expr(LinExpr<Key1> &&t_expr) : m_linear(std::move(t_expr)) {

}

template<class Key1, class Key2, class ValueT>
idol::Expr<Key1, Key2, ValueT>::Expr(const LinExpr<Key1> &t_expr) : m_linear(t_expr) {

}

template<class Key1, class Key2, class ValueT>
idol::Expr<Key1, Key2, ValueT>::Expr(const Expr &t_src)
        : m_linear(t_src.m_linear),
          m_constant(t_src.m_constant) {

}

template<class Key1, class Key2, class ValueT>
idol::Expr<Key1, Key2, ValueT> &idol::Expr<Key1, Key2, ValueT>::operator=(const Expr &t_rhs) {
    if (this == &t_rhs) { return *this; }
    m_linear = t_rhs.m_linear;
    m_constant = t_rhs.m_constant;
    return *this;
}

template<class Key1, class Key2, class ValueT>
idol::Expr<Key1, Key2, ValueT> &idol::Expr<Key1, Key2, ValueT>::operator+=(const Expr &t_rhs) {
    m_linear += t_rhs.m_linear;
    m_constant += t_rhs.m_constant;
    return *this;
}

template<class Key1, class Key2, class ValueT>
idol::Expr<Key1, Key2, ValueT> &idol::Expr<Key1, Key2, ValueT>::operator-=(const Expr &t_rhs) {
    m_linear -= t_rhs.m_linear;
    m_constant -= t_rhs.m_constant;
    return *this;
}

template<class Key1, class Key2, class ValueT>
idol::Expr<Key1, Key2, ValueT> &idol::Expr<Key1, Key2, ValueT>::operator*=(double t_rhs) {
    m_linear *= t_rhs;
    m_constant *= t_rhs;
    return *this;
}

template<class Key1, class Key2, class ValueT>
idol::Expr<Key1, Key2, ValueT> &idol::Expr<Key1, Key2, ValueT>::operator/=(double t_rhs) {
    m_linear /= t_rhs;
    m_constant /= t_rhs;
    return *this;
}

namespace idol {

    template<class Key1, class Key2, class ValueT>
    std::ostream &operator<<(std::ostream &t_os, const idol::Expr<Key1, Key2, ValueT> &t_expr) {

        if (std::abs(t_expr.constant()) < Tolerance::Sparsity) {

            t_os << t_expr.linear();

            return t_os;
        }

        t_os << t_expr.constant();

        if (!t_expr.linear().empty()) {
            t_os << " + " << t_expr.linear();
        }

        return t_os;
    }

}

#endif //IDOL_EXPR_H
