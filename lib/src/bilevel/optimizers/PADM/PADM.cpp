//
// Created by Henri on 22/04/2026.
//
#include "idol/bilevel/optimizers/PADM/PADM.h"
#include "idol/bilevel/optimizers/PADM/Optimizers_PADM.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

idol::Bilevel::PADM::PADM(const Bilevel::Description &t_description) : m_description(&t_description) {

}

idol::Optimizer *idol::Bilevel::PADM::create(const idol::Model &t_model) const {

    if (!m_single_level_optimizer) {
        throw Exception("No deterministic optimizer has been set.");
    }

    if (!m_description) {
        throw Exception("No bilevel description has been set.");
    }

    auto* result = new Optimizers::Bilevel::PADM(t_model,
                                                *m_description,
                                                *m_single_level_optimizer);

    return result;
}

idol::OptimizerFactory *idol::Bilevel::PADM::clone() const {
    return new PADM(*this);
}

idol::Bilevel::PADM &
idol::Bilevel::PADM::with_single_level_optimizer(const idol::OptimizerFactory &t_deterministic_optimizer) {
    if (m_single_level_optimizer) {
        throw Exception("PADM optimizer has already been set.");
    }
    m_single_level_optimizer.reset(t_deterministic_optimizer.clone());
    return *this;
}

idol::Bilevel::PADM::PADM(const idol::Bilevel::PADM &t_src)
        : OptimizerFactoryWithDefaultParameters<PADM>(t_src),
          m_description(t_src.m_description),
          m_single_level_optimizer(t_src.m_single_level_optimizer ? t_src.m_single_level_optimizer->clone() : nullptr) {

}

void idol::Bilevel::PADM::set_bilevel_description(const idol::Bilevel::Description &t_bilevel_description) {
    m_description = &t_bilevel_description;
}
