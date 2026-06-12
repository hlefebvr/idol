//
// Created by Henri on 01/06/2026.
//
#include <idol/robust/optimizers/bilevel-based-branch-and-bound/MaxMinRelaxation.h>

#include "idol/robust/optimizers/bilevel-based-branch-and-bound/Optimizers_MaxMinRelaxation.h"

idol::Robust::MaxMinRelaxation::MaxMinRelaxation(const Robust::Description& t_description, const Bilevel::Description& t_bilevel_description) :
    m_description(t_description),
    m_bilevel_description(t_bilevel_description) {

}

idol::Optimizer* idol::Robust::MaxMinRelaxation::create(const Model& t_model) const {

    if (!m_master_optimizer_factory) {
        throw Exception("No optimizer for solving the master problem was configured.");
    }

    if (!m_deterministic_optimizer_factory) {
        throw Exception("No optimizer for solving the deterministic problem was configured.");
    }

    auto* result = new Optimizers::Robust::MaxMinRelaxation(
        t_model,
        m_description,
        m_bilevel_description,
        *m_master_optimizer_factory,
        *m_deterministic_optimizer_factory,
        m_use_indicator.value_or(false)
    );

    return result;
}

idol::Robust::MaxMinRelaxation::MaxMinRelaxation(const MaxMinRelaxation& t_src) :
    OptimizerFactoryWithDefaultParameters<idol::Robust::MaxMinRelaxation>(t_src),
    m_description(t_src.m_description),
    m_bilevel_description(t_src.m_bilevel_description),
    m_use_indicator(t_src.m_use_indicator),
    m_master_optimizer_factory(t_src.m_master_optimizer_factory ? t_src.m_master_optimizer_factory->clone() : nullptr),
    m_deterministic_optimizer_factory(t_src.m_deterministic_optimizer_factory ? t_src.m_deterministic_optimizer_factory->clone() : nullptr) {

}

idol::OptimizerFactory* idol::Robust::MaxMinRelaxation::clone() const {
    return new idol::Robust::MaxMinRelaxation(*this);
}

idol::Robust::MaxMinRelaxation& idol::Robust::MaxMinRelaxation::with_master_optimizer(const OptimizerFactory& t_optimizer) {

    if (m_master_optimizer_factory) {
        throw Exception("An optimizer for solving the master problem has already been configured.");
    }

    m_master_optimizer_factory.reset(t_optimizer.clone());

    return *this;
}

idol::Robust::MaxMinRelaxation& idol::Robust::MaxMinRelaxation::with_deterministic_optimizer(const OptimizerFactory& t_optimizer) {

    if (m_deterministic_optimizer_factory) {
        throw Exception("An optimizer for solving the deterministic problem has already been configured.");
    }

    m_deterministic_optimizer_factory.reset(t_optimizer.clone());

    return *this;
}

idol::Robust::MaxMinRelaxation& idol::Robust::MaxMinRelaxation::with_indicator(bool t_value) {

    if (m_use_indicator) {
        throw Exception("The use of indicator has already been configured.");
    }

    m_use_indicator = t_value;

    return *this;
}
