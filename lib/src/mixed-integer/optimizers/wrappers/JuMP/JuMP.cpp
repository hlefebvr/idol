//
// Created by henri on 02.05.25.
//

#include "idol/mixed-integer/optimizers/wrappers/JuMP/JuMP.h"
#include "idol/mixed-integer/optimizers/wrappers/JuMP/Optimizers_JuMP.h"

idol::JuMP::JuMP(std::string t_module, bool t_is_continuous_relaxation)
        : OptimizerFactoryWithDefaultParameters(),
          m_module(std::move(t_module)),
          m_is_continuous_relaxation(t_is_continuous_relaxation) {

}

idol::JuMP::JuMP(std::string t_module) : JuMP(std::move(t_module), false) {

}

idol::Optimizer *idol::JuMP::operator()(const idol::Model &t_model) const {

    auto* result = new Optimizers::JuMP(t_model,
                                        m_module,
                                        m_optimizer_name.value_or(m_module + ".Optimizer"),
                                        m_is_continuous_relaxation
                                        );

    handle_default_parameters(result);

    return result;
}

idol::OptimizerFactory *idol::JuMP::clone() const {
    return new JuMP(*this);
}

idol::JuMP idol::JuMP::ContinuousRelaxation(std::string t_module) {
    return {std::move(t_module), true};
}
