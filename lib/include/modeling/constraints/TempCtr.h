//
// Created by henri on 08/09/22.
//

#ifndef OPTIMIZE_TEMPCTR_H
#define OPTIMIZE_TEMPCTR_H

#include "modeling/columns_and_rows/Deprecated_Row.h"

namespace impl {
    class Ctr;
}

namespace Solution {
    class Primal;
}

class TempCtr {
    friend class impl::Ctr;
    Deprecated_Row m_row;
    CtrType m_type;
public:
    TempCtr(Deprecated_Row&& t_row, CtrType t_type) : m_row(std::move(t_row)), m_type(t_type) {}

    const Deprecated_Row& row() const { return m_row; }

    Deprecated_Row& row() { return m_row; }

    CtrType type() const { return m_type; }

    bool is_violated(const Solution::Primal& t_solution) const;
};

TempCtr operator<=(Deprecated_Expr t_expr, Constant t_rhs);
TempCtr operator>=(Deprecated_Expr t_expr, Constant t_rhs);
TempCtr operator==(Deprecated_Expr t_expr, Constant t_rhs);

std::ostream& operator<<(std::ostream& t_os, const TempCtr& t_temp_ctr);

#endif //OPTIMIZE_TEMPCTR_H
