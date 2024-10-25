//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_TEMPVAR_H
#define OPTIMIZE_TEMPVAR_H

#include "idol/mixed-integer/modeling/matrix/Column.h"

namespace idol {
    class TempVar;
}

/**
 * Temporary variable class.
 *
 * This class represents a variable which has not been built yet by a Model.
 * It contains all the arguments needed to create a real variable
 * as instantiated by the `Var` class. It is made of a lower and an upper bound, a type (see `VarType`),
 * and a column (see `Column`).
 *
 * **Example 1**:
 * ```
 * model.add_var(0., 1., Binary, 2.);
 * // is the same as
 * model.add_var(TempVar(0., 1., Binary, 2.));
 * ```
 */
class idol::TempVar {
    double m_lb = 0.;
    double m_ub = Inf;
    VarType m_type = Continuous;
    std::unique_ptr<Column> m_column;
protected:
    void set_column(Column&& t_column) { m_column = std::make_unique<Column>(std::move(t_column)); }
    bool has_column() const { return (bool) m_column; }
    void reset_column() { m_column.reset(); }
public:
    /**
     * Default constructor.
     *
     * Creates a new temporary variable. The default lower and upper bounds are `0` and `Inf` and the type is `Continuous`.
     */
    TempVar() = default;

    /**
     * Constructor.
     * 
     * Creates a new temporary variable.
     * @param t_lb The desired lower bound.
     * @param t_ub The desired upper bound.
     * @param t_type The desired variable type.
     * @param t_column The desired column.
     */
    TempVar(double t_lb, double t_ub, VarType t_type, Column&& t_column) : m_lb(t_lb), m_ub(t_ub), m_type(t_type), m_column(std::make_unique<Column>(std::move(t_column))) {}
    
    /**
     * Copy constructor.
     * @param t_src The object to copy.
     */
    TempVar(TempVar&& t_src) = default;

    /**
     * Move constructor.
     * @param t_src The object to move.
     */
    TempVar(const TempVar& t_src) : m_lb(t_src.m_lb), m_ub(t_src.m_ub), m_type(t_src.m_type), m_column(t_src.m_column ? std::make_unique<Column>(*t_src.m_column) : std::unique_ptr<Column>()) {}

    /**
     * Copy-assignment operator.
     * @param t_src The object to copy.
     */
    TempVar& operator=(TempVar&& t_src) = default;

    /**
     * Move-assignment operator.
     * @param t_src The object to move.
     */
    TempVar& operator=(const TempVar& t_src) = default;

    /**
     * Returns the column of the temporary variable (see `Column`).
     * @return The column of the temporary variable.
     */
    [[nodiscard]] const Column& column() const { return *m_column; }
    
    /**
     * Returns the column of the temporary variable (see `Column`).
     * @return The column of the temporary variable.
     */
    Column& column() { return *m_column; }

    /**
     * Returns the lower bound of the temporary variable.
     * @return The lower bound of the temporary variable.
     */
    [[nodiscard]] double lb() const { return m_lb; }

    /**
     * Sets the lower bound of the temporary variable.
     * @param t_lb The desired lower bound of the temporary variable.
     */
    void set_lb(double t_lb) { m_lb = t_lb; }

    /**
     * Returns the upper bound of the temporary variable.
     * @return The upper bound of the temporary variable.
     */
    [[nodiscard]] double ub() const { return m_ub; }

    /**
     * Sets the upper bound of the temporary variable.
     * @param t_ub The desired upper bound of the temporary variable.
     */
    void set_ub(double t_ub) { m_ub = t_ub; }

    /**
     * Returns the type of the temporary variable.
     * @return The type of the temporary variable.
     */
    [[nodiscard]] VarType type() const { return m_type; }

    /**
     * Sets the type of the temporary variable.
     * @param t_type The desired type of the temporary variable.
     */
    void set_type(VarType t_type) {
        if (t_type < 0 || t_type > 2) {
            throw Exception("Variable type out of bounds.");
        }
        m_type = t_type;
    }
};

#endif //OPTIMIZE_TEMPVAR_H
