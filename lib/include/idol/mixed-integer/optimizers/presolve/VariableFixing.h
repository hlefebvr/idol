//
// Created by Henri on 02/04/2026.
//

#ifndef IDOL_VARIABLEFIXING_H
#define IDOL_VARIABLEFIXING_H

#include "AbstractPresolver.h"
#include "idol/mixed-integer/modeling/expressions/LinExpr.h"

namespace idol::Presolvers {
    class VariableFixing;
}

class idol::Presolvers::VariableFixing : public AbstractPresolver {
    unsigned int m_n_total_changes = 0;
public:
    bool execute(Model& t_model) override;
    void log_after_termination() const override;
    [[nodiscard]] AbstractPresolver* clone() const override;
};

#endif //IDOL_VARIABLEFIXING_H