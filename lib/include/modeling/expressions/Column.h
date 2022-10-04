//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_COLUMN_H
#define OPTIMIZE_COLUMN_H

#include <memory>
#include "modeling/matrix/AbstractMatrixCoefficient.h"
#include "Expr.h"
#include "modeling/constraints/Constraint.h"

class Column {
    Expr<Ctr> m_components;
    std::unique_ptr<AbstractMatrixCoefficient> m_objective_coefficient;
public:
    Column() = default;
    explicit Column(Constant t_objective_coefficient);

    Column(const Column& t_src);
    Column(Column&& t_src) noexcept = default;

    Column& operator=(const Column& t_src);
    Column& operator=(Column&& t_src) noexcept = default;

    Expr<Ctr>& components() { return m_components; }
    const Expr<Ctr>& components() const { return m_components; }

    Constant& objective_coefficient() { return m_objective_coefficient->value(); }
    const Constant& objective_coefficient() const { return m_objective_coefficient->value(); }

    void set_components(Expr<Ctr> t_components);
    void set_objective_coefficient(Constant t_objective_coefficient);

    static const Column EmptyColumn;
};

#endif //OPTIMIZE_COLUMN_H
