//
// Created by Henri on 02/04/2026.
//

#ifndef IDOL_BOUNDROUNDING_H
#define IDOL_BOUNDROUNDING_H
#include "AbstractPresolver.h"

namespace idol::Presolvers {
    class BoundRounding;
}

class idol::Presolvers::BoundRounding : public AbstractPresolver {
    unsigned int m_n_total_changes = 0;
public:
    bool execute(Model& t_model) override;
    void log_after_termination() const override;
    [[nodiscard]] AbstractPresolver* clone() const override;
};

#endif //IDOL_BOUNDROUNDING_H