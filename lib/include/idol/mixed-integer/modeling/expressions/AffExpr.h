//
// Created by henri on 21/10/22.
//

#ifndef IDOL_AFFEXPR_H
#define IDOL_AFFEXPR_H

#include "LinExpr.h"
#include "idol/general/numericals.h"

namespace idol {
    class Var;

    template<class Key1, class ValueT>
    class AffExpr;
}

template<class Key1 = idol::Var, class ValueT = double>
class idol::AffExpr {
    LinExpr<Key1, ValueT> m_linear;
    double m_constant = 0.;
public:
    AffExpr();
    AffExpr(const ValueT& t_constant); // NOLINT(google-explicit-constructor)
    AffExpr(ValueT&& t_constant); // NOLINT(google-explicit-constructor)
    AffExpr(const Key1& t_var); // NOLINT(google-explicit-constructor)
    AffExpr(LinExpr<Key1>&& t_expr); // NOLINT(google-explicit-constructor)
    AffExpr(const LinExpr<Key1>& t_expr); // NOLINT(google-explicit-constructor)

    virtual ~AffExpr() = default;

    AffExpr(const AffExpr& t_src) = default;
    AffExpr(AffExpr&&) noexcept = default;

    AffExpr& operator=(const AffExpr& t_rhs) = default;
    AffExpr& operator=(AffExpr&&) noexcept = default;

    AffExpr& operator+=(const AffExpr& t_rhs);
    AffExpr& operator-=(const AffExpr& t_rhs);
    AffExpr& operator*=(double t_rhs);
    AffExpr& operator/=(double t_rhs);
    AffExpr operator-() const;

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

template<class Key1, class ValueT>
idol::AffExpr<Key1, ValueT> idol::AffExpr<Key1, ValueT>::operator-() const {
    auto result = *this;
    result.constant() = -result.constant();
    result.linear() = -result.linear();
    return result;
}

template<class Key1, class ValueT>
idol::AffExpr<Key1, ValueT>::AffExpr() {

}

template<class Key1, class ValueT>
idol::AffExpr<Key1, ValueT>::AffExpr(const ValueT& t_constant) : m_constant(t_constant) {

}

template<class Key1, class ValueT>
idol::AffExpr<Key1, ValueT>::AffExpr(ValueT&& t_constant) : m_constant(t_constant) {

}

template<class Key1, class ValueT>
idol::AffExpr<Key1, ValueT>::AffExpr(const Key1 &t_var) : m_linear(t_var) {

}

template<class Key1, class ValueT>
idol::AffExpr<Key1, ValueT>::AffExpr(LinExpr<Key1> &&t_expr) : m_linear(std::move(t_expr)) {

}

template<class Key1, class ValueT>
idol::AffExpr<Key1, ValueT>::AffExpr(const LinExpr<Key1> &t_expr) : m_linear(t_expr) {

}

template<class Key1, class ValueT>
idol::AffExpr<Key1, ValueT> &idol::AffExpr<Key1, ValueT>::operator+=(const AffExpr &t_rhs) {
    m_linear += t_rhs.m_linear;
    m_constant += t_rhs.m_constant;
    return *this;
}

template<class Key1, class ValueT>
idol::AffExpr<Key1, ValueT> &idol::AffExpr<Key1, ValueT>::operator-=(const AffExpr &t_rhs) {
    m_linear -= t_rhs.m_linear;
    m_constant -= t_rhs.m_constant;
    return *this;
}

template<class Key1, class ValueT>
idol::AffExpr<Key1, ValueT> &idol::AffExpr<Key1, ValueT>::operator*=(double t_rhs) {
    m_linear *= t_rhs;
    m_constant *= t_rhs;
    return *this;
}

template<class Key1, class ValueT>
idol::AffExpr<Key1, ValueT> &idol::AffExpr<Key1, ValueT>::operator/=(double t_rhs) {
    m_linear /= t_rhs;
    m_constant /= t_rhs;
    return *this;
}

namespace idol {

    template<class Key1, class ValueT>
    std::ostream &operator<<(std::ostream &t_os, const idol::AffExpr<Key1, ValueT> &t_expr) {

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

#endif //IDOL_AFFEXPR_H