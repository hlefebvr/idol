//
// Created by henri on 19.11.24.
//

#ifndef IDOL_TEMPQCTR_H
#define IDOL_TEMPQCTR_H

#include "idol/mixed-integer/modeling/expressions/QuadExpr.h"

namespace idol {
    class TempQCtr;
}

class idol::TempQCtr {
    QuadExpr<Var> m_expr;
    CtrType m_type = LessOrEqual;
public:
    TempQCtr() = default;

    TempQCtr(QuadExpr<Var>&& t_expr, CtrType t_type) : m_expr(std::move(t_expr)), m_type(t_type) {}

    TempQCtr(const TempQCtr& t_src) = default;

    TempQCtr(TempQCtr&&) noexcept = default;

    TempQCtr& operator=(const TempQCtr& t_rhs) = default;

    TempQCtr& operator=(TempQCtr&&) noexcept = default;

    QuadExpr<Var>& expr() { return m_expr; }

    [[nodiscard]] const QuadExpr<Var>& expr() const { return m_expr; }

    [[nodiscard]] CtrType type() const { return m_type; }

    void set_expr(QuadExpr<Var>&& t_expr) { m_expr = std::move(t_expr); }

    void set_type(CtrType t_type) { m_type = t_type; }
    
};

idol::TempQCtr operator<=(idol::QuadExpr<idol::Var, double>&& t_lhs, idol::QuadExpr<idol::Var, double>&& t_rhs);
idol::TempQCtr operator<=(const idol::QuadExpr<idol::Var, double>& t_lhs, idol::QuadExpr<idol::Var, double>&& t_rhs);
idol::TempQCtr operator<=(idol::QuadExpr<idol::Var, double>&& t_lhs, const idol::QuadExpr<idol::Var, double>& t_rhs);
idol::TempQCtr operator<=(const idol::QuadExpr<idol::Var, double>& t_lhs, const idol::QuadExpr<idol::Var, double>& t_rhs);

idol::TempQCtr operator>=(idol::QuadExpr<idol::Var, double>&& t_lhs, idol::QuadExpr<idol::Var, double>&& t_rhs);
idol::TempQCtr operator>=(const idol::QuadExpr<idol::Var, double>& t_lhs, idol::QuadExpr<idol::Var, double>&& t_rhs);
idol::TempQCtr operator>=(idol::QuadExpr<idol::Var, double>&& t_lhs, const idol::QuadExpr<idol::Var, double>& t_rhs);
idol::TempQCtr operator>=(const idol::QuadExpr<idol::Var, double>& t_lhs, const idol::QuadExpr<idol::Var, double>& t_rhs);

idol::TempQCtr operator==(idol::QuadExpr<idol::Var, double>&& t_lhs, idol::QuadExpr<idol::Var, double>&& t_rhs);
idol::TempQCtr operator==(const idol::QuadExpr<idol::Var, double>& t_lhs, idol::QuadExpr<idol::Var, double>&& t_rhs);
idol::TempQCtr operator==(idol::QuadExpr<idol::Var, double>&& t_lhs, const idol::QuadExpr<idol::Var, double>& t_rhs);
idol::TempQCtr operator==(const idol::QuadExpr<idol::Var, double>& t_lhs, const idol::QuadExpr<idol::Var, double>& t_rhs);

namespace idol {
    std::ostream &operator<<(std::ostream &t_os, const TempQCtr &t_temp_ctr);
}

#endif //IDOL_TEMPQCTR_H
