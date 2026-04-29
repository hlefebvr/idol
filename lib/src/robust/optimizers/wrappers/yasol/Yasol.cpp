//
// Created by Henri on 29/04/2026.
//
#include "idol/robust/optimizers/wrappers/yasol/Yasol.h"
#include "idol/robust/optimizers/wrappers/yasol/Optimizers_Yasol.h"

idol::Robust::Yasol::Yasol(const Robust::Description& t_robust_description, const Bilevel::Description& t_bilevel_description) :
    m_robust_description(t_robust_description),
    m_bilevel_description(&t_bilevel_description) {

}

idol::Robust::Yasol::Yasol(const Robust::Description& t_robust_description) : m_robust_description(t_robust_description) {

}

idol::OptimizerFactory* idol::Robust::Yasol::clone() const {
    return new Yasol(*this);
}

idol::Optimizer* idol::Robust::Yasol::create(const Model& t_model) const {

    auto* result = new Optimizers::Robust::Yasol(
        t_model,
        m_robust_description,
        m_bilevel_description
    );

    return result;
}