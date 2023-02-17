//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_ROW_H
#define OPTIMIZE_ROW_H

#include <memory>
#include "AbstractMatrixCoefficient.h"
#include "../expressions/Expr.h"

namespace impl {
    class Row;
}

class impl::Row {
    ::Expr<Var, Var> m_impl;
protected:
    ::Expr<Var, Var>& impl() { return m_impl; }
public:
    Row() = default;
    Row(::Expr<Var, Var>&& t_lhs, ::Expr<Var, Var>&& t_rhs);
    Row(::Expr<Var, Var>&& t_lhs, const ::Expr<Var, Var>& t_rhs);
    Row(const ::Expr<Var, Var>& t_lhs, ::Expr<Var, Var>&& t_rhs);
    Row(const ::Expr<Var, Var>& t_lhs, const ::Expr<Var, Var>& t_rhs);

    Row(const Row& t_src) = default;
    Row(Row&& t_src) noexcept = default;

    Row& operator=(const Row& t_src) = default;
    Row& operator=(Row&& t_src) noexcept = default;

    Constant& rhs() { return m_impl.constant(); }
    const Constant& rhs() const { return m_impl.constant(); }

    LinExpr<Var>& linear() { return m_impl.linear(); }
    const LinExpr<Var>& linear() const { return m_impl.linear(); }

    QuadExpr<Var, Var>& quadratic() { return m_impl.quadratic(); }
    const QuadExpr<Var, Var>& quadratic() const { return m_impl.quadratic(); }

    void set_linear(LinExpr<Var>&& t_lin_expr) {  m_impl.linear() = std::move(t_lin_expr); }
    void set_linear(const LinExpr<Var>& t_lin_expr) {  m_impl.linear() = t_lin_expr; }

    void set_quadratic(QuadExpr<Var, Var>&& t_quad_expr) { m_impl.quadratic() = std::move(t_quad_expr); }
    void set_quadratic(const QuadExpr<Var, Var>& t_quad_expr) { m_impl.quadratic() = t_quad_expr; }

    void set_rhs(Constant&& t_rhs) { m_impl.constant() = std::move(t_rhs); }
    void set_rhs(const Constant& t_rhs) { m_impl.constant() = t_rhs; }

    Row& operator+=(const Row& t_rhs) { m_impl += t_rhs.m_impl; return *this; }
    Row& operator-=(const Row& t_rhs) { m_impl -= t_rhs.m_impl; return *this; }
    Row& operator*=(double t_rhs) { m_impl *= t_rhs; return *this; }
};

/**
 * Row modeling object.
 *
 * This class is used to represent an optimization model's row. It is made of a right handside and a set of Var-Constant
 * pairs representing the Constant coefficient of the row in each column Var. Such pairs are called left handside terms.
 *
 * The whole left handside is stored as an Expr while the whole right handside is stored as a Constant.
 */
class Row : public impl::Row {
    friend class Matrix;
public:
    Row() = default;
    Row(Expr<Var, Var>&& t_lhs, Expr<Var, Var>&& t_rhs) : impl::Row(std::move(t_lhs), std::move(t_rhs)) {}
    Row(Expr<Var, Var>&& t_lhs, const Expr<Var, Var>& t_rhs) : impl::Row(std::move(t_lhs), t_rhs) {}
    Row(const Expr<Var, Var>& t_lhs, Expr<Var, Var>&& t_rhs) : impl::Row(t_lhs, std::move(t_rhs)) {}
    Row(const Expr<Var, Var>& t_lhs, const Expr<Var, Var>& t_rhs) : impl::Row(t_lhs, t_rhs) {}

    Row(const Row& t_src) = default;
    Row(Row&& t_src) noexcept = default;

    Row& operator=(const Row& t_src) = default;
    Row& operator=(Row&& t_src) noexcept = default;

    [[nodiscard]] Row fix(const Solution::Primal& t_primals) const;
    [[nodiscard]] Row fix(const Solution::Dual& t_duals) const;

    static const Row EmptyRow;
};

static std::ostream &operator<<(std::ostream& t_os, const Row& t_row) {

    t_os << '[';

    if (t_row.linear().empty()) {

        t_os << t_row.quadratic();

    } else {

        t_os << t_row.linear();

        if (!t_row.quadratic().empty()) {
            t_os << " + " << t_row.quadratic();
        }

    }

    return t_os << "] [" << t_row.rhs() << ']';
}

#endif //OPTIMIZE_ROW_H
