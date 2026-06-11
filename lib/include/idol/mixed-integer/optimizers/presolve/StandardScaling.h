//
// Created by Henri on 11/06/2026.
//

#ifndef IDOL_STANDARDSCALING_H
#define IDOL_STANDARDSCALING_H

#include "AbstractPresolver.h"

namespace idol::Presolvers {
    class StandardScaling;
}

/**
 * This class implements standard scaling (a.k.a. equilibration scaling) of the model.
 * For now, it is only performed row-wise since there is no post-processing implemented yet.
 */
class idol::Presolvers::StandardScaling : public AbstractPresolver {
public:
    bool execute(Model& t_model) override;

    [[nodiscard]] AbstractPresolver* clone() const override { return new StandardScaling(*this); }
};

#endif //IDOL_STANDARDSCALING_H