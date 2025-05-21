//
// Created by henri on 02.05.25.
//

#ifndef IDOL_JUMP_H
#define IDOL_JUMP_H

#include "idol/general/optimizers/OptimizerFactory.h"

namespace idol {
    class JuMP;
}

class idol::JuMP : public idol::OptimizerFactoryWithDefaultParameters<JuMP> {
    const std::string m_module;
    std::optional<std::string> m_optimizer_name;
    bool m_is_continuous_relaxation = false;

    JuMP(std::string t_module, bool t_is_continuous_relaxation);
public:
    explicit JuMP(std::string t_module = "HiGHS");

    static JuMP ContinuousRelaxation(std::string t_module = "HiGHS");

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;
};

#endif //IDOL_JUMP_H
