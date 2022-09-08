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
};

#endif //OPTIMIZE_IMPL_VARIABLE_H
