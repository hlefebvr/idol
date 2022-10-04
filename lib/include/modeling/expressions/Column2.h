//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_COLUMN2_H
#define OPTIMIZE_COLUMN2_H

#include <memory>
#include "modeling/coefficients/AbstractMatrixCoefficient.h"
#include "LinExpr.h"
#include "modeling/constraints/Constraint.h"

class Column2 {
    LinExpr<Ctr> m_components;
    std::unique_ptr<AbstractMatrixCoefficient> m_objective_coefficient;
public:
    Column2() = default;
    explicit Column2(Constant t_objective_coefficient);

    Column2(const Column2& t_src);
    Column2(Column2&& t_src) noexcept = default;

    Column2& operator=(const Column2& t_src);
    Column2& operator=(Column2&& t_src) noexcept = default;

    LinExpr<Ctr>& components() { return m_components; }
    const LinExpr<Ctr>& components() const { return m_components; }

    Constant& objective_coefficient() { return m_objective_coefficient->value(); }
    const Constant& objective_coefficient() const { return m_objective_coefficient->value(); }

    static const Column2 EmptyColumn;
};

#endif //OPTIMIZE_COLUMN2_H
