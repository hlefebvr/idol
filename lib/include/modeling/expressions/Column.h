//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_COLUMN_H
#define OPTIMIZE_COLUMN_H

#include <memory>
#include "../matrix/AbstractMatrixCoefficient.h"
#include "LinExpr.h"
#include "../constraints/Ctr.h"
#include "QuadExpr.h"
#include "Expr.h"

/**
 * Column modeling object.
 *
 * This class is used to represent an optimization model's column. It is made of an objective coefficient and a set of Ctr-Constant
 * pairs representing the Constant coefficient of the column in each row Ctr. Such pairs are called components.
 */
class Column {
    friend class Matrix;
    Expr<Ctr, Var> m_components;
public:
    /**
     * Creates an empty column.
     */
    Column();

    /**
     * Creates a new column with an objective coefficient and no components.
     * @param t_objective_coefficient
     */
    explicit Column(Constant t_objective_coefficient);

    Column(const Column& t_src);
    Column(Column&& t_src) noexcept = default;

    Column& operator=(const Column& t_src);
    Column& operator=(Column&& t_src) noexcept = default;

    /**
     * Returns the components of the column.
     */
    Expr<Ctr, Var>& components() { return m_components; }

    /**
     * Returns the components of the column.
     */
    const Expr<Ctr, Var>& components() const { return m_components; }

    /**
     * Returns the objective coefficient of the column.
     */
    Constant& objective_coefficient() { return m_components.constant(); }

    /**
     * Returns the objective coefficient of the column.
     */
    const Constant& objective_coefficient() const { return m_components.constant(); }

    /**
     * Sets the components to the ones given as argument. This erases and replaces previous components.
     * @param t_components The desired new components.
     */
    void set_components(LinExpr<Ctr> t_components);

    /**
     * Sets the objective coefficient.
     * @param t_objective_coefficient The desired new objective coefficient.
     */
    void set_objective_coefficient(Constant t_objective_coefficient);

    void set_objective_coefficient(MatrixCoefficientReference&& t_objective_coefficient);

    Column fix(const Solution::Primal& t_primals) const;

    static const Column EmptyColumn;
};

#endif //OPTIMIZE_COLUMN_H
