//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_TEMPVAR_H
#define OPTIMIZE_TEMPVAR_H

#include "idol/mixed-integer/modeling/expressions/LinExpr.h"
#include "idol/mixed-integer/modeling/constraints/Ctr.h"
#include "idol/mixed-integer/modeling/Types.h"

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
    double m_obj = 0.;
    LinExpr<Ctr> m_column;
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
    TempVar(double t_lb, double t_ub, VarType t_type, double t_obj, LinExpr<Ctr>&& t_column) : m_lb(t_lb), m_ub(t_ub), m_type(t_type), m_obj(t_obj), m_column(std::move(t_column)) {}
    
    /**
     * Copy constructor.
     * @param t_src The object to copy.
     */
    TempVar(TempVar&& t_src) = default;

    /**
     * Move constructor.
     * @param t_src The object to move.
     */
    TempVar(const TempVar& t_src) : m_lb(t_src.m_lb), m_ub(t_src.m_ub), m_type(t_src.m_type), m_obj(t_src.m_obj), m_column(t_src.m_column) {}

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
    [[nodiscard]] const LinExpr<Ctr>& column() const { return m_column; }
    
    /**
     * Returns the column of the temporary variable (see `Column`).
     * @return The column of the temporary variable.
     */
    LinExpr<Ctr>& column() { return m_column; }

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

    double obj() const { return m_obj; }

    void set_obj(double t_obj) { m_obj = t_obj; }
};

#endif //OPTIMIZE_TEMPVAR_H
