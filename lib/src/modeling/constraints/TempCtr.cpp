//
// Created by henri on 08/09/22.
//
#include "modeling/constraints/TempCtr.h"

TempCtr operator<=(Expr t_expr, Coefficient t_rhs) {
    return { Row(std::move(t_expr), std::move(t_rhs)), LessOrEqual };
}

TempCtr operator>=(Expr t_expr, Coefficient t_rhs) {
    return { Row(std::move(t_expr), std::move(t_rhs)), GreaterOrEqual };
}

TempCtr operator==(Expr t_expr, Coefficient t_rhs) {
    return { Row(std::move(t_expr), std::move(t_rhs)), Equal };
}
