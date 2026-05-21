//
// Created by henri on 02.05.25.
//

#ifndef IDOL_JUMP_H
#define IDOL_JUMP_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include <list>

namespace idol {
    class JuMP;
}

class idol::JuMP : public idol::OptimizerFactoryWithDefaultParameters<JuMP> {
    std::list<std::string> m_modules;
    std::optional<std::string> m_optimizer_name;
    bool m_is_continuous_relaxation = false;

    JuMP(const std::string& t_optimizer_name, bool t_is_continuous_relaxation);
protected:
    [[nodiscard]] Optimizer *create(const Model &t_model) const override;
public:
    explicit JuMP(const std::string& t_optimizer_name);

    JuMP& with_julia_module(const std::string& t_module);

    static JuMP ContinuousRelaxation(const std::string& t_optimizer);

    [[nodiscard]] OptimizerFactory *clone() const override;
};

#endif //IDOL_JUMP_H
