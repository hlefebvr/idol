//
// Created by henri on 08/09/22.
//
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/general/utils/Point.h"
#include "idol/mixed-integer/modeling/expressions/AffExpr.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

using namespace idol;

std::ostream &idol::operator<<(std::ostream& t_os, const TempCtr& t_temp_ctr) {
    t_os << t_temp_ctr.lhs();
    switch (t_temp_ctr.type()) {
        case LessOrEqual: t_os << " <= "; break;
        case GreaterOrEqual: t_os << " >= "; break;
        case Equal: t_os << " == "; break;
    }
    return t_os << t_temp_ctr.rhs();
}

idol::TempCtr operator<=(idol::AffExpr<idol::Var, double> t_lhs, idol::AffExpr<idol::Var, double> t_rhs) {
    auto diff = std::move(t_lhs) - std::move(t_rhs);
    return {std::move(diff.linear()), LessOrEqual, -diff.constant() };
}

idol::TempCtr operator>=(idol::AffExpr<idol::Var, double> t_lhs, idol::AffExpr<idol::Var, double> t_rhs) {
    auto diff = std::move(t_lhs) - std::move(t_rhs);
    return {std::move(diff.linear()), GreaterOrEqual, -diff.constant() };
}

idol::TempCtr operator==(idol::AffExpr<idol::Var, double> t_lhs, idol::AffExpr<idol::Var, double> t_rhs) {
    auto diff = std::move(t_lhs) - std::move(t_rhs);
    return {std::move(diff.linear()), Equal, -diff.constant() };
}
