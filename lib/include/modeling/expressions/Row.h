//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_ROW_H
#define OPTIMIZE_ROW_H

#include <memory>
#include "modeling/matrix/AbstractMatrixCoefficient.h"
#include "Expr.h"

class Row {
    Expr<Var> m_lhs;
    std::unique_ptr<AbstractMatrixCoefficient> m_rhs;
public:
    Row();
    Row(Expr<Var> t_lhs, Constant t_rhs);

    Row(const Row& t_src);
    Row(Row&& t_src) noexcept = default;

    Row& operator=(const Row& t_src);
    Row& operator=(Row&& t_src) noexcept = default;

    Expr<Var>& lhs() { return m_lhs; }
    const Expr<Var>& lhs() const { return m_lhs; }

    Constant& rhs() { return m_rhs->value(); }
    const Constant& rhs() const { return m_rhs->value(); }

    void set_lhs(Expr<Var> t_lhs);
    void set_rhs(Constant t_rhs);

    Row operator*=(double t_factor);
    Row operator+=(const Row& t_factor);

    static const Row EmptyRow;
};

static std::ostream &operator<<(std::ostream& t_os, const Row& t_row) {
    return t_os << "TODO";
}

#endif //OPTIMIZE_ROW_H
