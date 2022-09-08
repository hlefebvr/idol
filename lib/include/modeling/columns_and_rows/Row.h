//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_ROW_H
#define OPTIMIZE_ROW_H

#include "ColumnOrRow.h"
#include "Expr.h"

class Row : public ColumnOrRow<Var> {
public:
    Row() = default;
    explicit Row(Coefficient t_rhs) : ColumnOrRow<Var>(std::move(t_rhs)) {}
    Row(Expr t_expr, Coefficient t_rhs) : ColumnOrRow<Var>(std::move(t_expr), std::move(t_rhs)) {}

    Row(const Row& t_src) = default;
    Row(Row&& t_src) noexcept = default;

    Row& operator=(const Row& t_src) = default;
    Row& operator=(Row&& t_src) noexcept = default;
};


#endif //OPTIMIZE_ROW_H
