//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_ROW_H
#define OPTIMIZE_ROW_H

#include <memory>
#include "AbstractMatrixCoefficient.h"
#include "idol/modeling/expressions/Expr.h"

namespace idol {

    namespace impl {
        class Row;
    }

    class Row;

}

class idol::impl::Row {
    ::idol::Expr<Var, Var> m_impl;
protected:
    ::idol::Expr<Var, Var>& impl() { return m_impl; }
public:
    Row() = default;
    Row(::idol::Expr<Var, Var>&& t_lhs, ::idol::Expr<Var, Var>&& t_rhs);
    Row(::idol::Expr<Var, Var>&& t_lhs, const ::idol::Expr<Var, Var>& t_rhs);
    Row(const ::idol::Expr<Var, Var>& t_lhs, ::idol::Expr<Var, Var>&& t_rhs);
    Row(const ::idol::Expr<Var, Var>& t_lhs, const ::idol::Expr<Var, Var>& t_rhs);

    Row(const Row& t_src) = default;
    Row(Row&& t_src) noexcept = default;

    Row& operator=(const Row& t_src) = default;
    Row& operator=(Row&& t_src) noexcept = default;

    /**
     * Returns the right hand-side of the row.
     * @return The right hand-side of the row.
     */
    Constant& rhs() { return m_impl.constant(); }

    /**
     * Returns the right hand-side of the row.
     * @return The right hand-side of the row.
     */
    [[nodiscard]] const Constant& rhs() const { return m_impl.constant(); }

    /**
     * Returns the linear part of the left hand-side of the row.
     * @return The linear part of the left hand-side of the row.
     */
    LinExpr<Var>& linear() { return m_impl.linear(); }

    /**
     * Returns the linear part of the left hand-side of the row.
     * @return The linear part of the left hand-side of the row.
     */
    [[nodiscard]] const LinExpr<Var>& linear() const { return m_impl.linear(); }

    /**
     * Returns the quadratic part of the left hand-side of the row.
     * @return The quadratic part of the left hand-side of the row.
     */
    QuadExpr<Var, Var>& quadratic() { return m_impl.quadratic(); }

    /**
     * Returns the quadratic part of the left hand-side of the row.
     * @return The quadratic part of the left hand-side of the row.
     */
    [[nodiscard]] const QuadExpr<Var, Var>& quadratic() const { return m_impl.quadratic(); }

    /**
     * Sets the linear part of the left hand-side of the row.
     * @param t_lin_expr The new linear part of the right hand-side.
     */
    void set_linear(LinExpr<Var>&& t_lin_expr) {  m_impl.linear() = std::move(t_lin_expr); }

    /**
     * Sets the linear part of the left hand-side of the row.
     * @param t_lin_expr The new linear part of the right hand-side.
     */
    void set_linear(const LinExpr<Var>& t_lin_expr) {  m_impl.linear() = t_lin_expr; }

    /**
     * Sets the quadratic part of the left hand-side of the row.
     * @param t_lin_expr The new quadratic part of the right hand-side.
     */
    void set_quadratic(QuadExpr<Var, Var>&& t_quad_expr) { m_impl.quadratic() = std::move(t_quad_expr); }

    /**
     * Sets the quadratic part of the left hand-side of the row.
     * @param t_lin_expr The new quadratic part of the right hand-side.
     */
    void set_quadratic(const QuadExpr<Var, Var>& t_quad_expr) { m_impl.quadratic() = t_quad_expr; }

    /**
     * Sets the right hand-side of the row.
     * @param t_rhs The new right hand-side of the row.
     */
    void set_rhs(Constant&& t_rhs) { m_impl.constant() = std::move(t_rhs); }

    /**
     * Sets the right hand-side of the row.
     * @param t_rhs The new right hand-side of the row.
     */
    void set_rhs(const Constant& t_rhs) { m_impl.constant() = t_rhs; }

    /**
     * Adds another row to the row (both the left and right hand-sides are added).
     * @param t_rhs The row to add.
     * @return *this
     */
    Row& operator+=(const Row& t_rhs) { m_impl += t_rhs.m_impl; return *this; }

    /**
     * Subtracts another row to the row (both the left and right hand-sides are subtracted).
     * @param t_rhs The row to subtarct.
     * @return *this
     */
    Row& operator-=(const Row& t_rhs) { m_impl -= t_rhs.m_impl; return *this; }

    /**
     * Multiplies all the entries of the row by a given factor (both the left and right hand-sides are multiplied).
     * @param t_rhs The factor used for multiplication.
     * @return *this
     */
    Row& operator*=(double t_rhs) { m_impl *= t_rhs; return *this; }
};

/**
 * Row modeling object.
 *
 * This class is used to represent an optimization model's row. It is made of a right hand-side and a set of `{ Var, Constant }`
 * pairs representing the coefficient (`Constant`) of the row in each column (`Var`). Such pairs are called left hand-side terms.
 *
 * The whole left hand-side is stored as an `Expr<Var, Var>` while the whole right hand-side is stored as a `Constant`.
 *
 * **Example**:
 * ```cpp
 * auto constraint = model.add_ctr(x + y <= 1.);
 * const auto& row = model.get_ctr_row(constraint); // Row(x + y, 1.)
 * ```
 */
class idol::Row : public impl::Row {
    friend class Matrix;
public:
    /*
     * Default constructor.
     *
     * Creates an empty row.
     */
    Row() = default;

    /**
     * Constructor.
     *
     * Creates a new row with a left hand-side `t_lhs` and a right hand-side `t_rhs`.
     * @param t_lhs The left hand-side.
     * @param t_rhs The right hand-side.
     */
    Row(Expr<Var, Var>&& t_lhs, Expr<Var, Var>&& t_rhs) : impl::Row(std::move(t_lhs), std::move(t_rhs)) {}

    /**
     * Constructor.
     *
     * Creates a new row with a left hand-side `t_lhs` and a right hand-side `t_rhs`.
     * @param t_lhs The left hand-side.
     * @param t_rhs The right hand-side.
     */
    Row(Expr<Var, Var>&& t_lhs, const Expr<Var, Var>& t_rhs) : impl::Row(std::move(t_lhs), t_rhs) {}

    /**
     * Constructor.
     *
     * Creates a new row with a left hand-side `t_lhs` and a right hand-side `t_rhs`.
     * @param t_lhs The left hand-side.
     * @param t_rhs The right hand-side.
     */
    Row(const Expr<Var, Var>& t_lhs, Expr<Var, Var>&& t_rhs) : impl::Row(t_lhs, std::move(t_rhs)) {}

    /**
     * Constructor.
     *
     * Creates a new row with a left hand-side `t_lhs` and a right hand-side `t_rhs`.
     * @param t_lhs The left hand-side.
     * @param t_rhs The right hand-side.
     */
    Row(const Expr<Var, Var>& t_lhs, const Expr<Var, Var>& t_rhs) : impl::Row(t_lhs, t_rhs) {}

    /**
     * Copy constructor.
     * @param t_src The row to copy.
     */
    Row(const Row& t_src) = default;

    /**
     * Move constructor.
     * @param t_src The row to move.
     */
    Row(Row&& t_src) noexcept = default;

    /**
     * Copy-assignment operator.
     * @param t_src The row to copy.
     * @return *this
     */
    Row& operator=(const Row& t_src) = default;

    /**
     * Move-assignment operator.
     * @param t_src The row to move.
     * @return *this
     */
    Row& operator=(Row&& t_src) noexcept = default;

    /**
     * Creates a new row in which all `Param` in each `Constant` are replaced by their corresponding values in `t_primals`
     * @param t_primals the primal values for the parameters.
     * @return the new row.
     */
    [[nodiscard]] Row fix(const Solution::Primal& t_primals) const;

    /**
     * Creates a new row in which all `Param` in each `Constant` are replaced by their corresponding values in `t_duals`
     * @param t_primals the dual values for the parameters.
     * @return the new row.
     */
    [[nodiscard]] Row fix(const Solution::Dual& t_duals) const;

    /**
     * Returns the value of the row if the value of each variable is taken in `t_primals`, i.e., the difference between
     * the left and the right hand-side.
     *
     * If parameters are encountered, an exception is thrown.
     * @param t_primals The primal values.
     * @return The value of the row.
     */
    double value(const Solution::Primal& t_primals) const;

    /**
     * Returns true if the point stored in `t_primals` violates the constraint formed by the row and a constraint type
     * `t_type` with tolerance `t_tolerance`.
     * @param t_primals The primal values.
     * @param t_type The type of the constraint.
     * @param t_tolerance The tolerance for feasibility.
     * @return True if the given point violates the row.
     */
    bool is_violated(const Solution::Primal& t_primals, CtrType t_type, double t_tolerance = Tolerance::Feasibility) const;

    /**
     * Represents an empty row.
     */
    static const Row EmptyRow;
};

namespace idol {

    static std::ostream &operator<<(std::ostream &t_os, const Row &t_row) {

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

}

#endif //OPTIMIZE_ROW_H
