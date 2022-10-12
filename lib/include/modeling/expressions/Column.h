//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_COLUMN_H
#define OPTIMIZE_COLUMN_H

#include <memory>
#include "../matrix/AbstractMatrixCoefficient.h"
#include "Expr.h"
#include "../constraints/Constraint.h"

/**
 * Column modeling object.
 *
 * This class is used to represent an optimization model's column. It is made of an objective coefficient and a set of Ctr-Constant
 * pairs representing the Constant coefficient of the column in each row Ctr. Such pairs are called components.
 */
class Column {
    Expr<Ctr> m_components;
    std::unique_ptr<AbstractMatrixCoefficient> m_objective_coefficient;
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
    Expr<Ctr>& components() { return m_components; }

    /**
     * Returns the components of the column.
     */
    const Expr<Ctr>& components() const { return m_components; }

    /**
     * Returns the objective coefficient of the column.
     */
    Constant& objective_coefficient() { return m_objective_coefficient->value(); }

    /**
     * Returns the objective coefficient of the column.
     */
    const Constant& objective_coefficient() const { return m_objective_coefficient->value(); }

    /**
     * Sets the components to the ones given as argument. This erases and replaces previous components.
     * @param t_components The desired new components.
     */
    void set_components(Expr<Ctr> t_components);

    /**
     * Sets the objective coefficient.
     * @param t_objective_coefficient The desired new objective coefficient.
     */
    void set_objective_coefficient(Constant t_objective_coefficient);

    Column fix(const Solution::Primal& t_primals) const;

    static const Column EmptyColumn;
};

#endif //OPTIMIZE_COLUMN_H
