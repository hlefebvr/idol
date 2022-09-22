//
// Created by henri on 08/09/22.
//

#ifndef OPTIMIZE_TEMPCTR_H
#define OPTIMIZE_TEMPCTR_H

#include "modeling/columns_and_rows/Row.h"

namespace impl {
    class Ctr;
}

namespace Solution {
    class Primal;
}

class TempCtr {
    friend class impl::Ctr;
    Row m_row;
    CtrType m_type;
public:
    TempCtr(Row&& t_row, CtrType t_type) : m_row(std::move(t_row)), m_type(t_type) {}

    const Row& row() const { return m_row; }

    Row& row() { return m_row; }

    CtrType type() const { return m_type; }

    bool is_violated(const Solution::Primal& t_solution) const;
};

TempCtr operator<=(Expr t_expr, Coefficient t_rhs);
TempCtr operator>=(Expr t_expr, Coefficient t_rhs);
TempCtr operator==(Expr t_expr, Coefficient t_rhs);

std::ostream& operator<<(std::ostream& t_os, const TempCtr& t_temp_ctr);

#endif //OPTIMIZE_TEMPCTR_H
