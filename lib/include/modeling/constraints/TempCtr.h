//
// Created by henri on 08/09/22.
//

#ifndef OPTIMIZE_TEMPCTR_H
#define OPTIMIZE_TEMPCTR_H

#include "../expressions/Row.h"

namespace impl {
    class Ctr;
}

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
 * model.add_constraint(x + y <= 1.);
 * ```
 * **Example 2** (equivalent to Example 1):
 * ```
 * ...
 * model.add_constraint(TempCtr(Row(x + y, 1.), LessOrEqual));
 * ```
 */
class TempCtr {
    friend class impl::Ctr;
    Row m_row;
    CtrType m_type;
public:
    /**
     * Creates a new temporary constraint.
     * @param t_row The desired row.
     * @param t_type The desired constraint type.
     */
    TempCtr(Row&& t_row, CtrType t_type) : m_row(std::move(t_row)), m_type(t_type) {}

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
    CtrType type() const { return m_type; }

    /**
     * Returns true if the temporary constraint is violated by the given solution, false otherwise.
     * @param t_solution The solution to check.
     */
    bool is_violated(const Solution::Primal& t_solution) const;
};

TempCtr operator<=(Expr<Var> t_expr, Constant t_rhs);
TempCtr operator>=(Expr<Var> t_expr, Constant t_rhs);
TempCtr operator==(Expr<Var> t_expr, Constant t_rhs);

std::ostream& operator<<(std::ostream& t_os, const TempCtr& t_temp_ctr);

#endif //OPTIMIZE_TEMPCTR_H
