//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_DEPRECATED_ROW_H
#define OPTIMIZE_DEPRECATED_ROW_H

#include "Deprecated_ColumnOrRow.h"
#include "Deprecated_Expr.h"

class Deprecated_Row : public Deprecated_ColumnOrRow<Var> {
public:
    Deprecated_Row() = default;
    explicit Deprecated_Row(Constant t_rhs) : Deprecated_ColumnOrRow<Var>(std::move(t_rhs)) {}
    Deprecated_Row(Deprecated_Expr t_expr, Constant t_rhs) : Deprecated_ColumnOrRow<Var>(std::move(t_expr), std::move(t_rhs)) {}

    Deprecated_Row(const Deprecated_Row& t_src) = default;
    Deprecated_Row(Deprecated_Row&& t_src) noexcept = default;

    Deprecated_Row& operator=(const Deprecated_Row& t_src) = default;
    Deprecated_Row& operator=(Deprecated_Row&& t_src) noexcept = default;

    static const Deprecated_Row EmptyRow;
};


#endif //OPTIMIZE_DEPRECATED_ROW_H
