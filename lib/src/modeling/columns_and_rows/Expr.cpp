//
// Created by henri on 08/09/22.
//
#include "modeling/columns_and_rows/Deprecated_Expr.h"

Deprecated_Expr::Deprecated_Expr(const Var &t_var) : Deprecated_AbstractExpr<Var>(1., t_var) {

}

Deprecated_Expr::Deprecated_Expr(Constant t_factor, const Var &t_var) : Deprecated_AbstractExpr<Var>(std::move(t_factor), t_var) {

}

Deprecated_Expr &Deprecated_Expr::operator+=(const Deprecated_Expr &t_expr) {
    Deprecated_AbstractExpr::operator+=(t_expr);
    return *this;
}

Deprecated_Expr &Deprecated_Expr::operator*=(double t_factor) {
    Deprecated_AbstractExpr::operator*=(t_factor);
    return *this;
}

Deprecated_Expr operator*(double t_factor, const Var& t_var) {
    return { t_factor, t_var };
}

Deprecated_Expr operator*(double t_factor, Deprecated_Expr t_expr) {
    Deprecated_Expr result(std::move(t_expr));
    result *= t_factor;
    return result;
}

Deprecated_Expr operator+(Deprecated_Expr t_a, const Deprecated_Expr& t_b) {
    Deprecated_Expr result(std::move(t_a));
    result += t_b;
    return result;
}

Deprecated_Expr operator*(Constant t_factor, const Var& t_var) {
    return { std::move(t_factor), t_var };
}
