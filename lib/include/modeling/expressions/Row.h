//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_ROW_H
#define OPTIMIZE_ROW_H

#include <memory>
#include "modeling/matrix/AbstractMatrixCoefficient.h"
#include "Expr.h"

/**
 * Row modeling object.
 *
 * This class is used to represent an optimization model's row. It is made of a right handside and a set of Var-Constant
 * pairs representing the Constant coefficient of the row in each column Var. Such pairs are called left handside terms.
 *
 * The whole left handside is stored as an Expr while the whole right handside is stored as a Constant.
 */
class Row {
    Expr<Var> m_lhs;
    std::unique_ptr<AbstractMatrixCoefficient> m_rhs;
public:
    /**
     * Creates an empty row.
     */
    Row();

    /**
     * Creates a new row with the given left and right handsides
     * @param t_lhs The desired left handside.
     * @param t_rhs The desired right handside.
     */
    Row(Expr<Var> t_lhs, Constant t_rhs);

    Row(const Row& t_src);
    Row(Row&& t_src) noexcept = default;

    Row& operator=(const Row& t_src);
    Row& operator=(Row&& t_src) noexcept = default;

    /**
     * Returns the left handside.
     */
    Expr<Var>& lhs() { return m_lhs; }

    /**
     * Returns the left handisde.
     */
    const Expr<Var>& lhs() const { return m_lhs; }

    /**
     * Returns the right handside.
     */
    Constant& rhs() { return m_rhs->value(); }

    /**
     * Returns the right handside.
     */
    const Constant& rhs() const { return m_rhs->value(); }

    /**
     * Sets the left handside to the expression given as argument.
     * @param t_lhs The desired left handside.
     */
    void set_lhs(Expr<Var> t_lhs);

    /**
     * Sets the right handside as the Constant given as argument.
     * @param t_rhs The desired right handside.
     */
    void set_rhs(Constant t_rhs);

    Row operator*=(double t_factor);
    Row operator+=(const Row& t_factor);

    static const Row EmptyRow;
};

static std::ostream &operator<<(std::ostream& t_os, const Row& t_row) {
    return t_os << "TODO";
}

#endif //OPTIMIZE_ROW_H
