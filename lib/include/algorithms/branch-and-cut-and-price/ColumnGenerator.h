//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATOR_H
#define OPTIMIZE_COLUMNGENERATOR_H

#include "containers/Map.h"
#include "modeling/constraints/Constraint.h"
#include "modeling/columns_and_rows/Row.h"

class ColumnGenerator {
    VarType m_type = Continuous;
    double m_lower_bound = 0.;
    double m_upper_bound = Inf;
    Map<Ctr, Row> m_values;
public:
    ColumnGenerator() = default;

    void set(const Ctr& t_ctr, Expr t_expr, double t_offset = 0.);

    const Row& get(const Ctr& t_ctr) const;

    using const_iterator = typename Map<Ctr, Row>::const_iterator;

    const_iterator begin() const { return m_values.begin(); }
    const_iterator end() const { return m_values.end(); }
    const_iterator cbegin() const { return m_values.begin(); }
    const_iterator cend() const { return m_values.end(); }

    VarType type() const { return m_type; }

    double lower_bound() const { return m_lower_bound; }

    double upper_bound() const { return m_upper_bound; }
};

#endif //OPTIMIZE_COLUMNGENERATOR_H
