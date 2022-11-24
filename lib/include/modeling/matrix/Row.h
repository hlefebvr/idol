//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_ROW_H
#define OPTIMIZE_ROW_H

#include <memory>
#include "AbstractMatrixCoefficient.h"
#include "../expressions/Expr.h"

template<class Key1, class Key2> class Expr;

/**
 * Row modeling object.
 *
 * This class is used to represent an optimization model's row. It is made of a right handside and a set of Var-Constant
 * pairs representing the Constant coefficient of the row in each column Var. Such pairs are called left handside terms.
 *
 * The whole left handside is stored as an Expr while the whole right handside is stored as a Constant.
 */
class Row {
    Expr<Var, Var> m_lhs;
public:
    /**
     * Creates an empty row.
     */
    Row() = default;

    Row(Expr<Var, Var>&& t_lhs, Expr<Var, Var>&& t_rhs);
    Row(Expr<Var, Var>&& t_lhs, const Expr<Var, Var>& t_rhs);
    Row(const Expr<Var, Var>& t_lhs, Expr<Var, Var>&& t_rhs);
    Row(const Expr<Var, Var>& t_lhs, const Expr<Var, Var>& t_rhs);

    Row(const Row& t_src) = default;
    Row(Row&& t_src) noexcept = default;

    Row& operator=(const Row& t_src) = default;
    Row& operator=(Row&& t_src) noexcept = default;

    /**
     * Returns the left handside.
     */
    Expr<Var, Var>& lhs() { return m_lhs; }

    /**
     * Returns the left handisde.
     */
    const Expr<Var, Var>& lhs() const { return m_lhs; }

    /**
     * Returns the right handside.
     */
    Constant& rhs() { return m_lhs.constant(); }

    /**
     * Returns the right handside.
     */
    const Constant& rhs() const { return m_lhs.constant(); }

    /**
     * Sets the left handside to the expression given as argument.
     * @param t_lhs The desired left handside.
     */
    void set_lhs(LinExpr<Var> t_lhs);

    /**
     * Sets the right handside as the Constant given as argument.
     * @param t_rhs The desired right handside.
     */
    void set_rhs(Constant t_rhs);

    void set_rhs(MatrixCoefficientReference&& t_rhs);

    Row operator*=(double t_factor);
    Row operator+=(const Row& t_factor);

    /**
     * Returns the transpose of the row.
     *
     * Variables therefore become parameters and parameters become variables.
     *
     * **Example 1**
     * \f$ (f + Q\bar\xi)^Tx \le b^T\bar\xi \f$ gives \f$ (b + Q^T\bar x)^T\xi \le f^T\bar x \f$
     */
    Row transpose() const;

    Row fix(const Solution::Primal& t_primals) const;

    static const Row EmptyRow;
};

static std::ostream &operator<<(std::ostream& t_os, const Row& t_row) {
    return t_os << '[' << t_row.lhs().linear() << "] [" << t_row.rhs() << ']';
}

#endif //OPTIMIZE_ROW_H
