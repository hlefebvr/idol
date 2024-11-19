//
// Created by henri on 15.11.24.
//

#ifndef IDOL_QUADEXPR_H
#define IDOL_QUADEXPR_H

#include "AffExpr.h"

namespace idol {
    template<class, class>
    class QuadExpr;
}

template<class KeyT = idol::Var, class ValueT = double>
class idol::QuadExpr : public LinExpr<CommutativePair<KeyT>, ValueT> {
    AffExpr<KeyT, ValueT> m_affine;
public:
    QuadExpr() = default;
    QuadExpr(const LinExpr<KeyT, ValueT>& t_expr) : m_affine(t_expr) {} // NOLINT(*-explicit-constructor)
    QuadExpr(LinExpr<KeyT, ValueT>&& t_expr) : m_affine(std::move(t_expr)) {} // NOLINT(*-explicit-constructor)
    QuadExpr(const AffExpr<KeyT, ValueT>& t_expr) : m_affine(t_expr) {} // NOLINT(*-explicit-constructor)
    QuadExpr(AffExpr<KeyT, ValueT>&& t_expr) : m_affine(std::move(t_expr)) {} // NOLINT(*-explicit-constructor)
    QuadExpr(const ValueT& t_factor, const KeyT& t_key1, const KeyT& t_key2) : LinExpr<CommutativePair<KeyT>, ValueT>(CommutativePair<KeyT>(t_key1, t_key2), t_factor) {}
    QuadExpr(ValueT&& t_factor, const KeyT& t_key1, const KeyT& t_key2) : LinExpr<CommutativePair<KeyT>, ValueT>(CommutativePair<KeyT>(t_key1, t_key2), std::move(t_factor)) {}
    QuadExpr(const ValueT& t_factor, const KeyT& t_key) : m_affine(t_factor, t_key) {}
    QuadExpr(ValueT&& t_factor, const KeyT& t_key) : m_affine(std::move(t_factor), t_key) {}

    QuadExpr(const QuadExpr& t_src) = default;
    QuadExpr(QuadExpr&&) noexcept = default;

    QuadExpr& operator=(const QuadExpr& t_rhs) = default;
    QuadExpr& operator=(QuadExpr&&) noexcept = default;

    QuadExpr& operator+=(const QuadExpr& t_rhs);
    QuadExpr& operator-=(const QuadExpr& t_rhs);
    QuadExpr& operator*=(double t_rhs);
    QuadExpr& operator/=(double t_rhs);
    QuadExpr operator-() const;

    AffExpr<KeyT, ValueT>& affine() { return m_affine; }
    const AffExpr<KeyT, ValueT>& affine() const { return m_affine; }

    bool is_zero(double t_tolerance);

    void clear();
};

template<class KeyT, class ValueT>
void idol::QuadExpr<KeyT, ValueT>::clear() {
    LinExpr<CommutativePair<KeyT>, ValueT>::clear();
    m_affine.clear();
}

template<class KeyT, class ValueT>
bool idol::QuadExpr<KeyT, ValueT>::is_zero(double t_tolerance) {
    return m_affine.is_zero(t_tolerance) && LinExpr<CommutativePair<KeyT>, ValueT>::is_zero(t_tolerance);
}

template<class KeyT, class ValueT>
idol::QuadExpr<KeyT, ValueT> idol::QuadExpr<KeyT, ValueT>::operator-() const {
    QuadExpr result;
    result -= *this;
    return result;
}

template<class KeyT, class ValueT>
idol::QuadExpr<KeyT, ValueT> &idol::QuadExpr<KeyT, ValueT>::operator/=(double t_rhs) {
    LinExpr<CommutativePair<KeyT>, ValueT>::operator/=(t_rhs);
    m_affine /= t_rhs;
    return *this;
}

template<class KeyT, class ValueT>
idol::QuadExpr<KeyT, ValueT> &idol::QuadExpr<KeyT, ValueT>::operator*=(double t_rhs) {
    LinExpr<CommutativePair<KeyT>, ValueT>::operator*=(t_rhs);
    m_affine *= t_rhs;
    return *this;
}

template<class KeyT, class ValueT>
idol::QuadExpr<KeyT, ValueT> &
idol::QuadExpr<KeyT, ValueT>::operator-=(const QuadExpr &t_rhs) {
    LinExpr<CommutativePair<KeyT>, ValueT>::operator-=(t_rhs);
    m_affine -= t_rhs.m_affine;
    return *this;
}

template<class KeyT, class ValueT>
idol::QuadExpr<KeyT, ValueT> &
idol::QuadExpr<KeyT, ValueT>::operator+=(const QuadExpr &t_rhs) {
    LinExpr<CommutativePair<KeyT>, ValueT>::operator+=(t_rhs);
    m_affine += t_rhs.m_affine;
    return *this;
}

#endif //IDOL_QUADEXPR_H
