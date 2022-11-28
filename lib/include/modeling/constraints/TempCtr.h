//
// Created by henri on 08/09/22.
//

#ifndef OPTIMIZE_TEMPCTR_H
#define OPTIMIZE_TEMPCTR_H

#include "../matrix/Row.h"

namespace Solution {
    class Primal;
}

/**
 * Temporary constraint object.
 *
 * This class represents a constraint which has not been built yet by a Model.
 * It contains all the arguments needed to create a real constraint
 * as instantiated by the Ctr class. It is made of a row (i.e., a Row) and a type (i.e., a CtrType).
 *
 * Typically, one creates a TempCtr by using the overloaded <=, >= and == operators between variable expressions (i.e., Expr) and
 * constant terms (i.e., Constant).
 *
 * **Example 1**:
 * ```
 * ...
 * model.add_ctr(x + y <= 1.);
 * ```
 * **Example 2** (equivalent to Example 1):
 * ```
 * ...
 * model.add_ctr(TempCtr(Row(x + y, 1.), LessOrEqual));
 * ```
 */
class TempCtr {
    Row m_row;
    int m_type = LessOrEqual;
public:
    TempCtr() = default;
    /**
     * Creates a new temporary constraint.
     * @param t_row The desired row.
     * @param t_type The desired constraint type.
     */
    TempCtr(Row&& t_row, int t_type) : m_row(std::move(t_row)), m_type(t_type) {}

    TempCtr(const TempCtr&) = default;
    TempCtr(TempCtr&&) noexcept = default;

    TempCtr& operator=(const TempCtr&) = default;
    TempCtr& operator=(TempCtr&&) noexcept = default;

    /**
     * Returns the row for the constraint (see Row).
     */
    const Row& row() const { return m_row; }

    /**
     * Returns the row for the constraint (see Row).
     */
    Row& row() { return m_row; }

    /**
     * Returns the constraint type.
     */
    int type() const { return m_type; }

    void set_type(int t_type) {
        if (t_type < 0 || t_type > 2) {
            throw Exception("Constraint type out of bounds.");
        }
        m_type = t_type;
    }

    /**
     * Returns true if the temporary constraint is violated by the given solution, false otherwise.
     * @param t_solution The solution to check.
     */
    bool is_violated(const Solution::Primal& t_solution) const;
};

template<class Key1, class Key2> class Expr;

TempCtr operator<=(Expr<Var, Var>&& t_lhs, Expr<Var, Var>&& t_rhs);
TempCtr operator<=(const Expr<Var, Var>& t_lhs, Expr<Var, Var>&& t_rhs);
TempCtr operator<=(Expr<Var, Var>&& t_lhs, const Expr<Var, Var>& t_rhs);
TempCtr operator<=(const Expr<Var, Var>& t_lhs, const Expr<Var, Var>& t_rhs);

TempCtr operator>=(Expr<Var, Var>&& t_lhs, Expr<Var, Var>&& t_rhs);
TempCtr operator>=(const Expr<Var, Var>& t_lhs, Expr<Var, Var>&& t_rhs);
TempCtr operator>=(Expr<Var, Var>&& t_lhs, const Expr<Var, Var>& t_rhs);
TempCtr operator>=(const Expr<Var, Var>& t_lhs, const Expr<Var, Var>& t_rhs);

TempCtr operator==(Expr<Var, Var>&& t_lhs, Expr<Var, Var>&& t_rhs);
TempCtr operator==(const Expr<Var, Var>& t_lhs, Expr<Var, Var>&& t_rhs);
TempCtr operator==(Expr<Var, Var>&& t_lhs, const Expr<Var, Var>& t_rhs);
TempCtr operator==(const Expr<Var, Var>& t_lhs, const Expr<Var, Var>& t_rhs);

std::ostream& operator<<(std::ostream& t_os, const TempCtr& t_temp_ctr);

#endif //OPTIMIZE_TEMPCTR_H
