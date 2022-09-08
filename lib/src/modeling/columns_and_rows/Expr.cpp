//
// Created by henri on 08/09/22.
//
#include "modeling/columns_and_rows/Expr.h"

Expr::Expr(const Var &t_var) : AbstractExpr<Var>(1., t_var) {

}

Expr::Expr(Coefficient t_factor, const Var &t_var) : AbstractExpr<Var>(std::move(t_factor), t_var) {

}

Expr &Expr::operator+=(const Expr &t_expr) {
    AbstractExpr::operator+=(t_expr);
    return *this;
}

Expr &Expr::operator*=(double t_factor) {
    AbstractExpr::operator*=(t_factor);
    return *this;
}

Expr operator*(double t_factor, const Var& t_var) {
    return { t_factor, t_var };
}

Expr operator*(double t_factor, Expr t_expr) {
    Expr result(std::move(t_expr));
    result *= t_factor;
    return result;
}

Expr operator+(Expr t_a, const Expr& t_b) {
    Expr result(std::move(t_a));
    result += t_b;
    return result;
}

Expr operator*(Coefficient t_factor, const Var& t_var) {
    return { std::move(t_factor), t_var };
}
