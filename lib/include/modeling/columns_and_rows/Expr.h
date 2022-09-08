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

    Expr(const Expr&) = default;
    Expr(Expr&&) noexcept = default;

    Expr& operator=(const Expr&) = default;
    Expr& operator=(Expr&&) noexcept = default;
};

#endif //OPTIMIZE_EXPR_H
