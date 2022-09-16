//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_TEMPVAR_H
#define OPTIMIZE_TEMPVAR_H

#include "modeling/columns_and_rows/Column.h"

namespace impl {
    class Var;
}

class TempVar {
    friend class impl::Var;
    double m_lb;
    double m_ub;
    VarType m_type;
    Column m_column;
public:
    TempVar(double t_lb, double t_ub, VarType t_type, Column&& t_column) : m_lb(t_lb), m_ub(t_ub), m_type(t_type), m_column(std::move(t_column)) {}

    const Column& column() const { return m_column; }

    [[nodiscard]] double lb() const { return m_lb; }

    [[nodiscard]] double ub() const { return m_ub; }

    [[nodiscard]] VarType type() const { return m_type; }
};

#endif //OPTIMIZE_TEMPVAR_H
