//
// Created by Henri on 17/04/2026.
//
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/CriticalValueColumnAndConstraintGeneration.h"

#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/Optimizers_CriticalValueColumnAndConstraintGeneration.h"

idol::Robust::CriticalValueColumnAndConstraintGeneration::CriticalValueColumnAndConstraintGeneration(const Robust::Description& t_description)
    : m_description(t_description) {

}

idol::Robust::CriticalValueColumnAndConstraintGeneration::CriticalValueColumnAndConstraintGeneration(const CriticalValueColumnAndConstraintGeneration& t_src)
    : OptimizerFactoryWithDefaultParameters(*this),
      m_description(t_src.m_description),
      m_master_optimizer_factory(t_src.m_master_optimizer_factory ? t_src.m_master_optimizer_factory->clone() : nullptr),
      m_deterministic_optimizer_factory(t_src.m_deterministic_optimizer_factory ? t_src.m_deterministic_optimizer_factory->clone() : nullptr) {

}

idol::OptimizerFactory* idol::Robust::CriticalValueColumnAndConstraintGeneration::clone() const {
    return new CriticalValueColumnAndConstraintGeneration(*this);
}

idol::Optimizer* idol::Robust::CriticalValueColumnAndConstraintGeneration::create(const Model& t_model) const {

    if (!m_master_optimizer_factory) {
        throw Exception("No master optimizer has been configured.");
    }

    if (!m_deterministic_optimizer_factory) {
        throw Exception("No deterministic optimizer has been configured.");
    }

    auto* result = new Optimizers::Robust::CriticalValueColumnAndConstraintGeneration(
        t_model,
        m_description,
        *m_master_optimizer_factory,
        *m_deterministic_optimizer_factory
    );

    return result;
}

idol::Robust::CriticalValueColumnAndConstraintGeneration& idol::Robust::CriticalValueColumnAndConstraintGeneration::with_master_optimizer(const OptimizerFactory& t_optimizer) {

    if (m_master_optimizer_factory) {
        throw Exception("A master optimizer has already been configured.");
    }

    m_master_optimizer_factory.reset(t_optimizer.clone());

    return *this;
}

idol::Robust::CriticalValueColumnAndConstraintGeneration& idol::Robust::CriticalValueColumnAndConstraintGeneration::with_deterministic_optimizer(const OptimizerFactory& t_optimizer) {

    if (m_deterministic_optimizer_factory) {
        throw Exception("A deterministic optimizer has already been configured.");
    }

    m_deterministic_optimizer_factory.reset(t_optimizer.clone());

    return *this;
}
