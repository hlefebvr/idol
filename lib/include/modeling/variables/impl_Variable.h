//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_IMPL_VARIABLE_H
#define OPTIMIZE_IMPL_VARIABLE_H

#include "../objects/impl_Object.h"
#include "../Types.h"
#include "../columns_and_rows/Column.h"

namespace impl {
    class Var;
}

class impl::Var : public impl::Object {
    double m_lb;
    double m_ub;
    VarType m_type;
    Column m_column;
public:
    Var(ObjectId&& t_id, double t_lb, double t_ub, VarType t_type, Column&& t_column);

    Column& column();

    const Column& column() const;

    void set_lb(double t_lb) { m_lb = t_lb; }

    void set_ub(double t_ub) { m_ub = t_ub; }

    void set_type(VarType t_type) { m_type = t_type; }

    double lb() const { return m_lb; }

    double ub() const { return m_ub; }

    VarType type() const { return m_type; }
};

#endif //OPTIMIZE_IMPL_VARIABLE_H
