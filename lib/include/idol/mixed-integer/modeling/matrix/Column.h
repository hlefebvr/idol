//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_COLUMN_H
#define OPTIMIZE_COLUMN_H

#include <memory>
#include "idol/mixed-integer/modeling/expressions/Expr.h"
#include "idol/mixed-integer/modeling/constraints/Ctr.h"

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
    Column(double t_obj) : m_impl(t_obj) {} // NOLINT(google-explicit-constructor)

    Column(const Column& t_src) = default;
    Column(Column&& t_src) noexcept = default;

    Column& operator=(const Column& t_src) = default;
    Column& operator=(Column&& t_src) noexcept = default;

    /**
     * Returns the (linear) objective coefficient of the column.
     *
     * For instance, a model's variable `x` with column whose objective is `2` implies that the model has a term in the
     * objective function which is `2 * x`.
     * @return The (linear) objective coefficient of the column
     */
    double& obj() { return m_impl.constant(); }

    /**
     * Returns the (linear) objective coefficient of the column.
     * For instance, a model's variable `x` with column whose objective is `2` implies that the model has a term in the
     * objective function which is `2 * x`.
     * @return The (linear) objective coefficient of the column
     */
    [[nodiscard]] double obj() const { return m_impl.constant(); }

    /**
     * Returns the (quadratic) objective coefficient of the column.
     *
     * For instance, a model's variable `x` with column whose objective is `2 * y` implies that the model has a term in the
     * objective function which is `2 * y * x`.
     * @return The (quadratic) objective coefficient of the column
     */
    ::idol::LinExpr<Var>& obj_quadratic() { return m_obj_quad; }

    /**
     * Returns the (quadratic) objective coefficient of the column.
     *
     * For instance, a model's variable `x` with column whose objective is `2 * y` implies that the model has a term in the
     * objective function which is `2 * y * x`.
     * @return The (quadratic) objective coefficient of the column
     */
    [[nodiscard]] const ::idol::LinExpr<Var>& obj_quadratic() const { return m_obj_quad; }

    /**
     * Returns the (linear) components of the column.
     *
     * For instance, a model's variable `x` with a column having a component `{ constraint, 2 }` implies that the model
     * has a constraint `constraint` with a term `2 * x`.
     * @return The (linear) components of the column.
     */
    LinExpr<Ctr>& linear() { return m_impl.linear(); }

    /**
     * Returns the (linear) components of the column.
     *
     * For instance, a model's variable `x` with a column having a component `{ constraint, 2 }` implies that the model
     * has a constraint `constraint` with a term `2 * x`.
     * @return The (linear) components of the column.
     */
    [[nodiscard]] const LinExpr<Ctr>& linear() const { return m_impl.linear(); }

    /**
     * Returns the (quadratic) components of the column.
     *
     * For instance, a model's variable `x` with a column having a component `{ constraint, 2 * y }` implies that the model
     * has a constraint `constraint` with a term `2 * y * x`.
     * @return The (quadratic) components of the column.
     */
    QuadExpr<Ctr, Var>& quadratic() { return m_impl.quadratic(); }

    /**
     * Returns the (quadratic) components of the column.
     *
     * For instance, a model's variable `x` with a column having a component `{ constraint, 2 * y }` implies that the model
     * has a constraint `constraint` with a term `2 * y * x`.
     * @return The (quadratic) components of the column.
     */
    [[nodiscard]] const QuadExpr<Ctr, Var>& quadratic() const { return m_impl.quadratic(); }

    /**
     * Sets the (linear) components of the column.
     * @param t_lin_expr The (linear) components of the column
     */
    void set_linear(LinExpr<Ctr>&& t_lin_expr) {  m_impl.linear() = std::move(t_lin_expr); }

    /**
     * Sets the (linear) components of the column.
     * @param t_lin_expr The (linear) components of the column
     */
    void set_linear(const LinExpr<Ctr>& t_lin_expr) {  m_impl.linear() = t_lin_expr; }

    /**
     * Sets the (quadratic) components of the column.
     * @param t_lin_expr The (quadratic) components of the column
     */
    void set_quadratic(QuadExpr<Ctr, Var>&& t_quad_expr) { m_impl.quadratic() = std::move(t_quad_expr); }

    /**
     * Sets the (quadratic) components of the column.
     * @param t_lin_expr The (quadratic) components of the column
     */
    void set_quadratic(const QuadExpr<Ctr, Var>& t_quad_expr) { m_impl.quadratic() = t_quad_expr; }

    /**
     * Sets the (linear) objective coefficient.
     * @param t_obj The (linear) objective coefficient
     */
    void set_obj(double t_obj) { m_impl.constant() = t_obj; }

    /**
     * Adds the objective and components of `t_rhs` to the objective and components of the column.
     * @param t_rhs the right hand-side
     * @return *this
     */
    Column& operator+=(const Column& t_rhs) { m_impl += t_rhs.m_impl; return *this; }

    /**
     * Subtracts the objective and components of `t_rhs` to the objective and components of the column.
     * @param t_rhs the right hand-side
     * @return *this
     */
    Column& operator-=(const Column& t_rhs) { m_impl -= t_rhs.m_impl; return *this; }

    /**
     * Multiplies the objective and components of the column by a given factor.
     * @param t_rhs the right hand-side
     * @return *this
     */
    Column& operator*=(double t_rhs) { m_impl *= t_rhs; return *this; }
};

/**
 * Column modeling-old object.
 *
 * This class is used to represent an optimization model's column. It is made of an objective coefficient and a set of
 * `{ Ctr, Constant }`
 * pairs representing the coefficient (`Constant`) of the column in each row (`Ctr`). Such pairs are called components.
 */
class idol::Column : public impl::Column {
public:
    /**
     * Default constructor.
     *
     * Creates an empty column.
     */
    Column() = default;

    /**
     * Constructor.
     *
     * Creates a new column with no components and on objective coefficient equal to `t_obj`
     * @param t_obj The objective coefficient
     */
    Column(double t_obj) : impl::Column(t_obj) {} // NOLINT(google-explicit-constructor)

    /**
     * Copy constructor.
     * @param t_src The object to be copied
     */
    Column(const Column& t_src) = default;

    /**
     * Move constructor.
     * @param t_src The object to be moved
     */
    Column(Column&& t_src) noexcept = default;

    /**
     * Copy-assignment operator
     * @param t_src The object to copy
     * @return *this
     */
    Column& operator=(const Column& t_src) = default;

    /**
     * Move-assignment operator
     * @param t_src The object to move
     * @return *this
     */
    Column& operator=(Column&& t_src) noexcept = default;

    /**
     * Represents an empty column
     */
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
