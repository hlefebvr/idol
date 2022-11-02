//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_TEMPVAR_H
#define OPTIMIZE_TEMPVAR_H

#include "../expressions/Column.h"

namespace impl {
    class Var;
}

/**
 * Temporary variable object.
 *
 * This class represents a variable which has not been built yet by a Model.
 * It contains all the arguments needed to create a real variable
 * as instantiated by the Var class. It is made of a lower and an upper bound, a type (see VarType),
 * and a column (see Column).
 *
 * **Example 1**:
 * ```
 * model.add_var(0., 1., Binary, 2.);
 * // is the same as
 * model.add_var(TempVar(0., 1., Binary, 2.));
 * ```
 */
class TempVar {
    friend class impl::Var;
    double m_lb;
    double m_ub;
    VarType m_type;
    Column m_column;
public:
    /**
     * Creates a new temporary variable.
     * @param t_lb The desired lower bound.
     * @param t_ub The desired upper bound.
     * @param t_type The desired variable type.
     * @param t_column The desired column.
     */
    TempVar(double t_lb, double t_ub, VarType t_type, Column&& t_column) : m_lb(t_lb), m_ub(t_ub), m_type(t_type), m_column(std::move(t_column)) {}

    /**
     * Returns the column associated to the temporary variable (see Column).
     */
    const Column& column() const { return m_column; }

    /**
     * Returns the column associated to the temporary variable (see Column).
     */
    Column& column() { return m_column; }

    /**
     * Returns the lower bound associated to the temporary variable.
     */
    [[nodiscard]] double lb() const { return m_lb; }

    /**
     * Returns the upper bound associated to the temporary variable.
     */
    [[nodiscard]] double ub() const { return m_ub; }

    /**
     * Returns the type associated to the temporary variable.
     */
    [[nodiscard]] VarType type() const { return m_type; }
};

#endif //OPTIMIZE_TEMPVAR_H
