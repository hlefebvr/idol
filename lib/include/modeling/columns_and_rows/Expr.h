//
// Created by henri on 08/09/22.
//

#ifndef OPTIMIZE_EXPR_H
#define OPTIMIZE_EXPR_H

#include "AbstractExpr.h"
#include "modeling/variables/Variable.h"

class Expr : public AbstractExpr<Var> {
public:
    Expr() = default;
    Expr(const Var& t_var); // NOLINT(google-explicit-constructor)
    Expr(Coefficient t_factor, const Var& t_var);

    Expr(const Expr&) = default;
    Expr(Expr&&) noexcept = default;

    Expr& operator=(const Expr&) = default;
    Expr& operator=(Expr&&) noexcept = default;

    Expr& operator*=(double t_factor) override;
    Expr& operator+=(const Expr& t_expr);
};

Expr operator*(double t_factor, const Var& t_var);
Expr operator*(Coefficient t_factor, const Var& t_var);
Expr operator*(double t_factor, Expr t_expr);
Expr operator+(Expr t_a, const Expr& t_b);

#endif //OPTIMIZE_EXPR_H
