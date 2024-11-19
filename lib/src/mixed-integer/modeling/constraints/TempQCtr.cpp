//
// Created by henri on 19.11.24.
//
#include <idol/mixed-integer/modeling/constraints/TempQCtr.h>

using namespace idol;

TempQCtr operator<=(QuadExpr<Var>&& t_lhs, QuadExpr<Var>&& t_rhs) {
    auto diff = std::move(t_lhs);
    diff -= std::move(t_rhs);
    return { std::move(diff), LessOrEqual };
}

TempQCtr operator<=(const QuadExpr<Var>& t_lhs, QuadExpr<Var>&& t_rhs) { return QuadExpr<Var>(t_lhs) <= std::move(t_rhs); }
TempQCtr operator<=(QuadExpr<Var>&& t_lhs, const QuadExpr<Var>& t_rhs) { return std::move(t_lhs) <= QuadExpr<Var>(t_rhs); }
TempQCtr operator<=(const QuadExpr<Var>& t_lhs, const QuadExpr<Var>& t_rhs) { return QuadExpr<Var>(t_lhs) <= QuadExpr<Var>(t_rhs); }

TempQCtr operator>=(QuadExpr<Var>&& t_lhs, QuadExpr<Var>&& t_rhs) {
    auto diff = std::move(t_lhs);
    diff -= std::move(t_rhs);
    return { std::move(diff), GreaterOrEqual };
}

TempQCtr operator>=(const QuadExpr<Var>& t_lhs, QuadExpr<Var>&& t_rhs) { return QuadExpr<Var>(t_lhs) >= std::move(t_rhs); }
TempQCtr operator>=(QuadExpr<Var>&& t_lhs, const QuadExpr<Var>& t_rhs) { return std::move(t_lhs) >= QuadExpr<Var>(t_rhs); }
TempQCtr operator>=(const QuadExpr<Var>& t_lhs, const QuadExpr<Var>& t_rhs) { return QuadExpr<Var>(t_lhs) >= QuadExpr<Var>(t_rhs); }

TempQCtr operator==(QuadExpr<Var>&& t_lhs, QuadExpr<Var>&& t_rhs) {
    auto diff = std::move(t_lhs);
    diff -= std::move(t_rhs);
    return { std::move(diff), Equal };
}

TempQCtr operator==(const QuadExpr<Var>& t_lhs, QuadExpr<Var>&& t_rhs) { return QuadExpr<Var>(t_lhs) == std::move(t_rhs); }
TempQCtr operator==(QuadExpr<Var>&& t_lhs, const QuadExpr<Var>& t_rhs) { return std::move(t_lhs) == QuadExpr<Var>(t_rhs); }
TempQCtr operator==(const QuadExpr<Var>& t_lhs, const QuadExpr<Var>& t_rhs) { return QuadExpr<Var>(t_lhs) == QuadExpr<Var>(t_rhs); }

std::ostream &idol::operator<<(std::ostream& t_os, const TempQCtr& t_temp_ctr) {
    t_os << t_temp_ctr.expr();
    switch (t_temp_ctr.type()) {
        case LessOrEqual: t_os << " <= 0"; break;
        case GreaterOrEqual: t_os << " >= 0"; break;
        case Equal: t_os << " == 0"; break;
    }
    return t_os;
}
