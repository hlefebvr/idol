//
// Created by henri on 23/03/23.
//

#ifndef IDOL_GLPK_H
#define IDOL_GLPK_H

#include "idol/optimizers/OptimizerFactory.h"

namespace idol {
    class GLPK;
}

class idol::GLPK : public OptimizerFactoryWithDefaultParameters<GLPK> {
    bool m_continuous_relaxation = false;

    explicit GLPK(bool t_continuous_relaxation) : m_continuous_relaxation(t_continuous_relaxation) {}
    GLPK(const GLPK&) = default;
public:
    GLPK() = default;
    GLPK(GLPK&&) noexcept = default;

    GLPK& operator=(const GLPK&) = delete;
    GLPK& operator=(GLPK&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    static GLPK ContinuousRelaxation();

    [[nodiscard]] GLPK *clone() const override;
};


#endif //IDOL_GLPK_H
