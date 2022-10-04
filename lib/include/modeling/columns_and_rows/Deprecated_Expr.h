//
// Created by henri on 08/09/22.
//

#ifndef OPTIMIZE_DEPRECATED_EXPR_H
#define OPTIMIZE_DEPRECATED_EXPR_H

#include "Deprecated_AbstractExpr.h"
#include "modeling/variables/Variable.h"

class Deprecated_Expr : public Deprecated_AbstractExpr<Var> {
public:
    Deprecated_Expr() = default;
    Deprecated_Expr(const Var& t_var); // NOLINT(google-explicit-constructor)
    Deprecated_Expr(Constant t_factor, const Var& t_var);

    Deprecated_Expr(const Deprecated_Expr&) = default;
    Deprecated_Expr(Deprecated_Expr&&) noexcept = default;

    Deprecated_Expr& operator=(const Deprecated_Expr&) = default;
    Deprecated_Expr& operator=(Deprecated_Expr&&) noexcept = default;

    Deprecated_Expr& operator*=(double t_factor) override;
    Deprecated_Expr& operator+=(const Deprecated_Expr& t_expr);
};

Deprecated_Expr operator*(double t_factor, const Var& t_var);
Deprecated_Expr operator*(Constant t_factor, const Var& t_var);
Deprecated_Expr operator*(double t_factor, Deprecated_Expr t_expr);
Deprecated_Expr operator+(Deprecated_Expr t_a, const Deprecated_Expr& t_b);

#endif //OPTIMIZE_DEPRECATED_EXPR_H
