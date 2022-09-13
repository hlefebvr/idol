//
// Created by henri on 12/09/22.
//

#ifndef OPTIMIZE_COLGENERATOR_H
#define OPTIMIZE_COLGENERATOR_H

#include "modeling/constraints/Constraint.h"
#include "modeling/columns_and_rows/Row.h"

class ColGenerator {
    Row m_objective;
    Map<Ctr, Row> m_column;
public:
    ColGenerator() = default;

    void set_objective(Row t_row);

    void set_objective(Expr t_expr, double t_constant = 0.);

    void set(const Ctr& t_ctr, Row t_row);

    void set(const Ctr& t_ctr, Expr t_expr, double t_constant = 0.);

    const Row& get(const Ctr& t_ctr) const;

    using const_iterator = Map<Ctr, Row>::const_iterator;

    const_iterator begin() const { return m_column.begin(); }

    const_iterator end() const { return m_column.begin(); }

    const_iterator cbegin() const { return m_column.begin(); }

    const_iterator cend() const { return m_column.begin(); }
};

#endif //OPTIMIZE_COLGENERATOR_H
