//
// Created by henri on 02.05.25.
//

#include "idol/mixed-integer/optimizers/wrappers/JuMP/JuMP.h"
#include "idol/mixed-integer/optimizers/wrappers/JuMP/Optimizers_JuMP.h"

idol::JuMP::JuMP(const std::string& t_optimizer_name, bool t_is_continuous_relaxation)
        : OptimizerFactoryWithDefaultParameters(),
          m_optimizer_name(t_optimizer_name),
          m_is_continuous_relaxation(t_is_continuous_relaxation) {

}

idol::JuMP::JuMP(const std::string& t_optimizer_name) : JuMP(t_optimizer_name, false) {

}

idol::JuMP& idol::JuMP::with_julia_module(const std::string& t_module) {
    m_modules.emplace_back(t_module);
    return *this;
}

idol::Optimizer *idol::JuMP::create(const idol::Model &t_model) const {

    if (!m_optimizer_name) {
        throw Exception("A JuMP optimizer is required.");
    }

    if (m_modules.empty()) {
        std::cerr << "Warning: You are asking for a JuMP optimizer without loading any module, most likely this will lead to an error." << std::endl;
    }

    auto* result = new Optimizers::JuMP(t_model,
                                        *m_optimizer_name,
                                        m_modules,
                                        m_is_continuous_relaxation
                                        );

    return result;
}

idol::OptimizerFactory *idol::JuMP::clone() const {
    return new JuMP(*this);
}

idol::JuMP idol::JuMP::ContinuousRelaxation(const std::string& t_optimizer) {
    return {t_optimizer, true};
}
