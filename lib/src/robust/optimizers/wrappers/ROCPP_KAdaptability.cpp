//
// Created by Henri on 13/05/2026.
//
#include "idol/robust/optimizers/wrappers/ROCPP_KAdaptability.h"
#include "idol/robust/optimizers/wrappers/Optimizers_ROCPP_KAdaptability.h"

idol::Robust::ROCPP::KAdaptability::KAdaptability(const Robust::Description& t_robust_description,
                                                  const Bilevel::Description& t_bilevel_description) :
    m_robust_description(t_robust_description),
    m_bilevel_description(t_bilevel_description) {

}

idol::OptimizerFactory* idol::Robust::ROCPP::KAdaptability::clone() const {
    return new KAdaptability(*this);
}

idol::Optimizer* idol::Robust::ROCPP::KAdaptability::create(const Model& t_model) const {

    auto* result = new Optimizers::Robust::ROCPP::KAdaptability(t_model, m_robust_description, m_bilevel_description);

    return result;
}
