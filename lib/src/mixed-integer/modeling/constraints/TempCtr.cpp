//
// Created by henri on 08/09/22.
//
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/general/utils/Point.h"
#include "idol/mixed-integer/modeling/expressions/Expr.h"

using namespace idol;

TempCtr operator<=(Expr<Var>&& t_lhs, Expr<Var>&& t_rhs) {
    auto diff = std::move(t_lhs);
    diff -= t_rhs;
    return { std::move(diff.linear()), LessOrEqual, diff.constant() };
}

TempCtr operator<=(const Expr<Var>& t_lhs, Expr<Var>&& t_rhs) { return Expr<Var>(t_lhs) <= std::move(t_rhs); }
TempCtr operator<=(Expr<Var>&& t_lhs, const Expr<Var>& t_rhs) { return std::move(t_lhs) <= Expr<Var>(t_rhs); }
TempCtr operator<=(const Expr<Var>& t_lhs, const Expr<Var>& t_rhs) { return Expr<Var>(t_lhs) <= Expr<Var>(t_rhs); }

TempCtr operator>=(Expr<Var>&& t_lhs, Expr<Var>&& t_rhs) {
    auto diff = std::move(t_lhs);
    diff -= t_rhs;
    return { std::move(diff.linear()), GreaterOrEqual, diff.constant() };
}

TempCtr operator>=(const Expr<Var>& t_lhs, Expr<Var>&& t_rhs) { return Expr<Var>(t_lhs) >= std::move(t_rhs); }
TempCtr operator>=(Expr<Var>&& t_lhs, const Expr<Var>& t_rhs) { return std::move(t_lhs) >= Expr<Var>(t_rhs); }
TempCtr operator>=(const Expr<Var>& t_lhs, const Expr<Var>& t_rhs) { return Expr<Var>(t_lhs) >= Expr<Var>(t_rhs); }

TempCtr operator==(Expr<Var>&& t_lhs, Expr<Var>&& t_rhs) {
    auto diff = std::move(t_lhs);
    diff -= t_rhs;
    return { std::move(diff.linear()), Equal, diff.constant() };
}

TempCtr operator==(const Expr<Var>& t_lhs, Expr<Var>&& t_rhs) { return Expr<Var>(t_lhs) == std::move(t_rhs); }
TempCtr operator==(Expr<Var>&& t_lhs, const Expr<Var>& t_rhs) { return std::move(t_lhs) == Expr<Var>(t_rhs); }
TempCtr operator==(const Expr<Var>& t_lhs, const Expr<Var>& t_rhs) { return Expr<Var>(t_lhs) == Expr<Var>(t_rhs); }

std::ostream &idol::operator<<(std::ostream& t_os, const TempCtr& t_temp_ctr) {
    t_os << t_temp_ctr.lhs();
    switch (t_temp_ctr.type()) {
        case LessOrEqual: t_os << " <= "; break;
        case GreaterOrEqual: t_os << " >= "; break;
        case Equal: t_os << " == "; break;
    }
    return t_os << t_temp_ctr.rhs();
}
