//
// Created by Henri on 02/04/2026.
//

#ifndef IDOL_ONEROWBOUNDTIGHTENING_H
#define IDOL_ONEROWBOUNDTIGHTENING_H

#include "AbstractPresolver.h"
#include "idol/mixed-integer/modeling/expressions/LinExpr.h"

namespace idol::Presolvers {
    class OneRowBoundTightening;
}

class idol::Presolvers::OneRowBoundTightening : public AbstractPresolver {
    unsigned int m_n_total_changes = 0;
    static unsigned int do_single_row_bound_tightening(Model& t_model, const LinExpr<Var>& t_row, CtrType t_type, double t_rhs);
public:
    bool execute(Model& t_model) override;
    void log_after_termination() const override;
    [[nodiscard]] AbstractPresolver* clone() const override;
};

#endif //IDOL_ONEROWBOUNDTIGHTENING_H