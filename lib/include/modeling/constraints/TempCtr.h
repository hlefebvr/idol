//
// Created by henri on 08/09/22.
//

#ifndef OPTIMIZE_TEMPCTR_H
#define OPTIMIZE_TEMPCTR_H

#include "modeling/columns_and_rows/Row.h"
#include "modeling/Types.h"

namespace impl {
    class Ctr;
}

class TempCtr {
    friend class impl::Ctr;
    Row m_row;
    CtrType m_type;
    TempCtr(Row&& t_row, CtrType t_type) : m_row(std::move(t_row)), m_type(t_type) {}

    friend TempCtr operator<=(Expr t_expr, Coefficient t_rhs);
    friend TempCtr operator>=(Expr t_expr, Coefficient t_rhs);
    friend TempCtr operator==(Expr t_expr, Coefficient t_rhs);
};

TempCtr operator<=(Expr t_expr, Coefficient t_rhs);
TempCtr operator>=(Expr t_expr, Coefficient t_rhs);
TempCtr operator==(Expr t_expr, Coefficient t_rhs);

#endif //OPTIMIZE_TEMPCTR_H
