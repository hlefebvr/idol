//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_COLUMN_H
#define OPTIMIZE_COLUMN_H

#include <memory>
#include "AbstractMatrixCoefficient.h"
#include "../expressions/Expr.h"

namespace impl {
    class Column;
}

class impl::Column {
    ::Expr<Ctr, Var> m_impl;
    ::LinExpr<Var> m_obj_quad;
protected:
    ::Expr<Ctr, Var>& impl() { return m_impl; }
public:
    Column() = default;
    Column(Constant&& t_obj) : m_impl(std::move(t_obj)) {} // NOLINT(google-explicit-constructor)
    Column(const Constant& t_obj) : m_impl(t_obj) {} // NOLINT(google-explicit-constructor)

    Column(const Column& t_src) = default;
    Column(Column&& t_src) noexcept = default;

    Column& operator=(const Column& t_src) = default;
    Column& operator=(Column&& t_src) noexcept = default;

    Constant& obj() { return m_impl.constant(); }
    const Constant& obj() const { return m_impl.constant(); }

    ::LinExpr<Var>& obj_quadratic() { return m_obj_quad; }
    const ::LinExpr<Var>& obj_quadratic() const { return m_obj_quad; }

    LinExpr<Ctr>& linear() { return m_impl.linear(); }
    const LinExpr<Ctr>& linear() const { return m_impl.linear(); }

    QuadExpr<Ctr, Var>& quadratic() { return m_impl.quadratic(); }
    const QuadExpr<Ctr, Var>& quadratic() const { return m_impl.quadratic(); }

    void set_linear(LinExpr<Ctr>&& t_lin_expr) {  m_impl.linear() = std::move(t_lin_expr); }
    void set_linear(const LinExpr<Ctr>& t_lin_expr) {  m_impl.linear() = t_lin_expr; }

    void set_quadratic(QuadExpr<Ctr, Var>&& t_quad_expr) { m_impl.quadratic() = std::move(t_quad_expr); }
    void set_quadratic(const QuadExpr<Ctr, Var>& t_quad_expr) { m_impl.quadratic() = t_quad_expr; }

    void set_obj(Constant&& t_obj) { m_impl.constant() = std::move(t_obj); }
    void set_obj(const Constant& t_obj) { m_impl.constant() = t_obj; }

    Column& operator+=(const Column& t_rhs) { m_impl += t_rhs.m_impl; return *this; }
    Column& operator-=(const Column& t_rhs) { m_impl -= t_rhs.m_impl; return *this; }
    Column& operator*=(double t_rhs) { m_impl *= t_rhs; return *this; }
};

/**
 * Column modeling object.
 *
 * This class is used to represent an optimization model's column. It is made of an objective coefficient and a set of Ctr-Constant
 * pairs representing the Constant coefficient of the column in each row Ctr. Such pairs are called components.
 */
class Column : public impl::Column {
    friend class Matrix;
public:
    Column() = default;
    Column(Constant&& t_obj) : impl::Column(std::move(t_obj)) {} // NOLINT(google-explicit-constructor)
    Column(const Constant& t_obj) : impl::Column(t_obj) {} // NOLINT(google-explicit-constructor)

    Column(const Column& t_src) = default;
    Column(Column&& t_src) noexcept = default;

    Column& operator=(const Column& t_src) = default;
    Column& operator=(Column&& t_src) noexcept = default;

    Column fix(const Solution::Primal& t_primals) const;

    static const Column EmptyColumn;
};

#endif //OPTIMIZE_COLUMN_H
