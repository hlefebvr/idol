//
// Created by henri on 23/03/23.
//

#ifndef IDOL_GLPK_H
#define IDOL_GLPK_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

namespace idol {
    class GLPK;
}

class idol::GLPK : public OptimizerFactoryWithDefaultParameters<GLPK> {
    bool m_continuous_relaxation = false;

    explicit GLPK(bool t_continuous_relaxation) : m_continuous_relaxation(t_continuous_relaxation) {}
public:
    GLPK() = default;

    GLPK(const GLPK&) = default;
    GLPK(GLPK&&) noexcept = default;

    GLPK& operator=(const GLPK&) = delete;
    GLPK& operator=(GLPK&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    static GLPK ContinuousRelaxation();

    [[nodiscard]] GLPK *clone() const override;

    static Model read_from_file(Env& t_env, const std::string& t_filename);
};


#endif //IDOL_GLPK_H
