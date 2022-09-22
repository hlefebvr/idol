//
// Created by henri on 08/09/22.
//
#include "modeling/constraints/TempCtr.h"
#include "solvers/solutions/Solution.h"

TempCtr operator<=(Expr t_expr, Coefficient t_rhs) {
    return { Row(std::move(t_expr), std::move(t_rhs)), LessOrEqual };
}

TempCtr operator>=(Expr t_expr, Coefficient t_rhs) {
    return { Row(std::move(t_expr), std::move(t_rhs)), GreaterOrEqual };
}

TempCtr operator==(Expr t_expr, Coefficient t_rhs) {
    return { Row(std::move(t_expr), std::move(t_rhs)), Equal };
}

bool TempCtr::is_violated(const Solution::Primal &t_solution) const {
    const double rhs = m_row.constant().constant();
    double lhs = 0.;
    for (const auto& [var, coeff] : m_row) {
        lhs += coeff.constant() * t_solution.get(var);
    }
    switch (m_type) {
        case LessOrEqual: return lhs <= rhs;
        case GreaterOrEqual: return lhs >= rhs;
        default:;
    }
    return equals(lhs, rhs, ToleranceForIntegrality);
}

std::ostream &operator<<(std::ostream& t_os, const TempCtr& t_temp_ctr) {
    t_os << (AbstractExpr<Var>&) t_temp_ctr.row();
    switch (t_temp_ctr.type()) {
        case LessOrEqual: t_os << " <= "; break;
        case GreaterOrEqual: t_os << " >= "; break;
        case Equal: t_os << " == "; break;
    }
    return t_os << t_temp_ctr.row().constant();
}