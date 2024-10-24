//
// Created by henri on 08/09/22.
//
#include "idol/modeling/constraints/TempCtr.h"
#include "idol/utils/Point.h"
#include "idol/modeling/expressions/LinExpr.h"

using namespace idol;

TempCtr operator<=(Expr<Var>&& t_lhs, Expr<Var>&& t_rhs) {
    return { Row(std::move(t_lhs), std::move(t_rhs)), LessOrEqual };
}

TempCtr operator<=(const Expr<Var>& t_lhs, Expr<Var>&& t_rhs) { return Expr<Var>(t_lhs) <= std::move(t_rhs); }
TempCtr operator<=(Expr<Var>&& t_lhs, const Expr<Var>& t_rhs) { return std::move(t_lhs) <= Expr<Var>(t_rhs); }
TempCtr operator<=(const Expr<Var>& t_lhs, const Expr<Var>& t_rhs) { return Expr<Var>(t_lhs) <= Expr<Var>(t_rhs); }

TempCtr operator>=(Expr<Var>&& t_lhs, Expr<Var>&& t_rhs) {
    return { Row(std::move(t_lhs), std::move(t_rhs)), GreaterOrEqual };
}

TempCtr operator>=(const Expr<Var>& t_lhs, Expr<Var>&& t_rhs) { return Expr<Var>(t_lhs) >= std::move(t_rhs); }
TempCtr operator>=(Expr<Var>&& t_lhs, const Expr<Var>& t_rhs) { return std::move(t_lhs) >= Expr<Var>(t_rhs); }
TempCtr operator>=(const Expr<Var>& t_lhs, const Expr<Var>& t_rhs) { return Expr<Var>(t_lhs) >= Expr<Var>(t_rhs); }

TempCtr operator==(Expr<Var>&& t_lhs, Expr<Var>&& t_rhs) {
    return { Row(std::move(t_lhs), std::move(t_rhs)), Equal };
}

TempCtr operator==(const Expr<Var>& t_lhs, Expr<Var>&& t_rhs) { return Expr<Var>(t_lhs) == std::move(t_rhs); }
TempCtr operator==(Expr<Var>&& t_lhs, const Expr<Var>& t_rhs) { return std::move(t_lhs) == Expr<Var>(t_rhs); }
TempCtr operator==(const Expr<Var>& t_lhs, const Expr<Var>& t_rhs) { return Expr<Var>(t_lhs) == Expr<Var>(t_rhs); }

bool TempCtr::is_violated(const PrimalPoint &t_solution) const {
    const double rhs = m_row.rhs();
    double lhs = 0.;
    for (const auto& [var, coeff] : m_row.linear()) {
        lhs += coeff * t_solution.get(var);
    }
    switch (m_type) {
        case LessOrEqual: return lhs > rhs;
        case GreaterOrEqual: return lhs < rhs;
        default:;
    }
    return equals(lhs, rhs, Tolerance::Feasibility);
}

std::ostream &idol::operator<<(std::ostream& t_os, const TempCtr& t_temp_ctr) {
    t_os << t_temp_ctr.row().linear();
    switch (t_temp_ctr.type()) {
        case LessOrEqual: t_os << " <= "; break;
        case GreaterOrEqual: t_os << " >= "; break;
        case Equal: t_os << " == "; break;
    }
    return t_os << t_temp_ctr.row().rhs();
}
