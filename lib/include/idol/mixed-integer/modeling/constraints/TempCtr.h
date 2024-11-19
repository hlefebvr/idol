//
// Created by henri on 08/09/22.
//

#ifndef OPTIMIZE_TEMPCTR_H
#define OPTIMIZE_TEMPCTR_H

#include <ostream>
#include <idol/mixed-integer/modeling/expressions/LinExpr.h>
#include <idol/mixed-integer/modeling/Types.h>
#include <idol/mixed-integer/modeling/variables/Var.h>

namespace idol {

    class TempCtr;

    template<class, class> class AffExpr;
}

/**
 * Temporary constraint class.
 *
 * This class represents a constraint which has not been built yet by a Model.
 * It contains all the arguments needed to create a real constraint
 * as instantiated by the `Ctr` class. It is made of a row (see `Row`) and a type (see `CtrType.rst.rst`).
 *
 * Typically, one creates a `TempCtr` by using the overloaded operators `<=`, `>=` and `==` between variable expressions (i.e., `AffExpr`) and
 * constant terms (i.e., `Constant`).
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
class idol::TempCtr {
    LinExpr<Var> m_lhs;
    CtrType m_type = LessOrEqual;
    double m_rhs = 0.;
public:
    /**
     * Default constructor.
     *
     * Creates an empty temporary constraint. The default type is `LessOrEqual`.
     */
    TempCtr() = default;

    /**
     * Constructor.
     *
     * Creates a new temporary constraint.
     * @param t_row The desired row.
     * @param t_type The desired constraint type.
     */
    TempCtr(LinExpr<Var>&& t_lhs, CtrType t_type, double t_rhs) : m_lhs(std::move(t_lhs)), m_rhs(t_rhs), m_type(t_type) {}

    /**
     * Copy constructor.
     * @param t_src The object to copy.
     */
    TempCtr(const TempCtr& t_src) = default;

    /**
     * Move constructor.
     * @param t_src The object to move.
     */
    TempCtr(TempCtr&& t_src) noexcept = default;

    /**
     * Copy-assignment operator.
     * @param t_src The object to copy.
     */
    TempCtr& operator=(const TempCtr& t_src) = default;

    /**
     * Move-assignment operator.
     * @param t_src The object to move.
     */
    TempCtr& operator=(TempCtr&& t_src) noexcept = default;

    LinExpr<Var>& lhs() { return m_lhs; }

    const LinExpr<Var>& lhs() const { return m_lhs; }

    void set_lhs(LinExpr<Var>&& t_lhs) { m_lhs = std::move(t_lhs); }

    /**
     * Returns the temporary constraint type.
     * @return The temporary constraint type.
     */
    [[nodiscard]] CtrType type() const { return m_type; }

    double rhs() const { return m_rhs; }

    double& rhs() { return m_rhs; }

    void set_rhs(double t_rhs) { m_rhs = t_rhs; }

    /**
     * Sets the type of the temporary constraint.
     * @param t_type the desired type.
     */
    void set_type(CtrType t_type) {
        if (t_type < 0 || t_type > 2) {
            throw Exception("Constraint type out of bounds.");
        }
        m_type = t_type;
    }

};

idol::TempCtr operator<=(idol::AffExpr<idol::Var, double>&& t_lhs, idol::AffExpr<idol::Var, double>&& t_rhs);
idol::TempCtr operator<=(const idol::AffExpr<idol::Var, double>& t_lhs, idol::AffExpr<idol::Var, double>&& t_rhs);
idol::TempCtr operator<=(idol::AffExpr<idol::Var, double>&& t_lhs, const idol::AffExpr<idol::Var, double>& t_rhs);
idol::TempCtr operator<=(const idol::AffExpr<idol::Var, double>& t_lhs, const idol::AffExpr<idol::Var, double>& t_rhs);

idol::TempCtr operator>=(idol::AffExpr<idol::Var, double>&& t_lhs, idol::AffExpr<idol::Var, double>&& t_rhs);
idol::TempCtr operator>=(const idol::AffExpr<idol::Var, double>& t_lhs, idol::AffExpr<idol::Var, double>&& t_rhs);
idol::TempCtr operator>=(idol::AffExpr<idol::Var, double>&& t_lhs, const idol::AffExpr<idol::Var, double>& t_rhs);
idol::TempCtr operator>=(const idol::AffExpr<idol::Var, double>& t_lhs, const idol::AffExpr<idol::Var, double>& t_rhs);

idol::TempCtr operator==(idol::AffExpr<idol::Var, double>&& t_lhs, idol::AffExpr<idol::Var, double>&& t_rhs);
idol::TempCtr operator==(const idol::AffExpr<idol::Var, double>& t_lhs, idol::AffExpr<idol::Var, double>&& t_rhs);
idol::TempCtr operator==(idol::AffExpr<idol::Var, double>&& t_lhs, const idol::AffExpr<idol::Var, double>& t_rhs);
idol::TempCtr operator==(const idol::AffExpr<idol::Var, double>& t_lhs, const idol::AffExpr<idol::Var, double>& t_rhs);

namespace idol {
    std::ostream &operator<<(std::ostream &t_os, const TempCtr &t_temp_ctr);
}

#endif //OPTIMIZE_TEMPCTR_H
