//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_ROW2_H
#define OPTIMIZE_ROW2_H

#include <memory>
#include "modeling/coefficients/AbstractMatrixCoefficient.h"
#include "LinExpr.h"

class Row2 {
    LinExpr<Var> m_lhs;
    std::unique_ptr<AbstractMatrixCoefficient> m_rhs;
public:
    Row2() = default;
    Row2(LinExpr<Var> t_lhs, Constant t_rhs);

    Row2(const Row2& t_src);
    Row2(Row2&& t_src) noexcept = default;

    Row2& operator=(const Row2& t_src);
    Row2& operator=(Row2&& t_src) noexcept = default;

    LinExpr<Var>& lhs() { return m_lhs; }
    const LinExpr<Var>& lhs() const { return m_lhs; }

    Constant& rhs() { return m_rhs->value(); }
    const Constant& rhs() const { return m_rhs->value(); }

    static const Row2 EmptyRow;
};

#endif //OPTIMIZE_ROW2_H
