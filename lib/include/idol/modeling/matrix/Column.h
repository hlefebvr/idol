//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_COLUMN_H
#define OPTIMIZE_COLUMN_H

#include <memory>
#include "AbstractMatrixCoefficient.h"
#include "idol/modeling/expressions/Expr.h"

namespace idol {

    namespace impl {
        class Column;
    }

    class Column;

}

class idol::impl::Column {
    ::idol::Expr<Ctr, Var> m_impl;
    ::idol::LinExpr<Var> m_obj_quad;
protected:
    ::idol::Expr<Ctr, Var>& impl() { return m_impl; }
public:
    Column() = default;
    Column(Constant&& t_obj) : m_impl(std::move(t_obj)) {} // NOLINT(google-explicit-constructor)
    Column(const Constant& t_obj) : m_impl(t_obj) {} // NOLINT(google-explicit-constructor)

    Column(const Column& t_src) = default;
    Column(Column&& t_src) noexcept = default;

    Column& operator=(const Column& t_src) = default;
    Column& operator=(Column&& t_src) noexcept = default;

    Constant& obj() { return m_impl.constant(); }
    [[nodiscard]] const Constant& obj() const { return m_impl.constant(); }

    ::idol::LinExpr<Var>& obj_quadratic() { return m_obj_quad; }
    [[nodiscard]] const ::idol::LinExpr<Var>& obj_quadratic() const { return m_obj_quad; }

    LinExpr<Ctr>& linear() { return m_impl.linear(); }
    [[nodiscard]] const LinExpr<Ctr>& linear() const { return m_impl.linear(); }

    QuadExpr<Ctr, Var>& quadratic() { return m_impl.quadratic(); }
    [[nodiscard]] const QuadExpr<Ctr, Var>& quadratic() const { return m_impl.quadratic(); }

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
class idol::Column : public impl::Column {
    friend class Matrix;
public:
    Column() = default;
    Column(Constant&& t_obj) : impl::Column(std::move(t_obj)) {} // NOLINT(google-explicit-constructor)
    Column(const Constant& t_obj) : impl::Column(t_obj) {} // NOLINT(google-explicit-constructor)

    Column(const Column& t_src) = default;
    Column(Column&& t_src) noexcept = default;

    Column& operator=(const Column& t_src) = default;
    Column& operator=(Column&& t_src) noexcept = default;

    [[nodiscard]] Column fix(const Solution::Primal& t_primals) const;

    static const Column EmptyColumn;
};

namespace idol {

    static std::ostream &operator<<(std::ostream& t_os, const idol::Column& t_column) {

        t_os << "[Obj: " << t_column.obj() << "]\n";

        for (const auto& [ctr, constant] : t_column.linear()) {
            t_os << '[' << ctr.name() << ": " << constant << "]\n";
        }

        return t_os;
    }

}

#endif //OPTIMIZE_COLUMN_H
