//
// Created by henri on 09.01.26.
//
#include <idol/robust/optimizers/critical-value-column-and-constraint-generation-algorithm/CriticalValueColumnAndConstraintGeneration.h>
#include <idol/robust/optimizers/critical-value-column-and-constraint-generation-algorithm/Optimizers_CriticalValueColumnAndConstraintGeneration.h>

idol::Robust::CriticalValueColumnAndConstraintGeneration::CriticalValueColumnAndConstraintGeneration(
    const CriticalValueColumnAndConstraintGeneration& t_src)
    : OptimizerFactoryWithDefaultParameters<idol::Robust::CriticalValueColumnAndConstraintGeneration>(*this),
      m_robust_description(t_src.m_robust_description),
      m_master_optimizer(t_src.m_master_optimizer ? t_src.m_master_optimizer->clone() : nullptr),
      m_separation_optimizer(t_src.m_separation_optimizer ? t_src.m_separation_optimizer->clone() : nullptr) {
}

idol::Robust::CriticalValueColumnAndConstraintGeneration::CriticalValueColumnAndConstraintGeneration(const Robust::Description& t_description)
    : m_robust_description(t_description) {

}

idol::Optimizer* idol::Robust::CriticalValueColumnAndConstraintGeneration::operator()(const Model& t_model) const {

    if (!m_master_optimizer) {
        throw Exception("No optimizer for solving the master problem has been set.");
    }

    if (!m_separation_optimizer) {
        throw Exception("No optimizer for solving the separation problem has been set.");
    }

    auto* result = new Optimizers::Robust::CriticalValueColumnAndConstraintGeneration(t_model, m_robust_description,
        *m_master_optimizer,
        *m_separation_optimizer
    );

    handle_default_parameters(result);

    return result;
}

idol::OptimizerFactory* idol::Robust::CriticalValueColumnAndConstraintGeneration::clone() const {
    return new CriticalValueColumnAndConstraintGeneration(*this);
}

idol::Robust::CriticalValueColumnAndConstraintGeneration&
idol::Robust::CriticalValueColumnAndConstraintGeneration::with_master_optimizer(const OptimizerFactory& t_factory) {

    if (m_master_optimizer) {
        throw Exception("An optimizer for solving the master problem has already been set.");
    }

    m_master_optimizer.reset(t_factory.clone());

    return *this;
}

idol::Robust::CriticalValueColumnAndConstraintGeneration&
idol::Robust::CriticalValueColumnAndConstraintGeneration::with_adversarial_optimizer(const OptimizerFactory& t_factory) {

    if (m_separation_optimizer) {
        throw Exception("An optimizer for solving the separation problem has already been set.");
    }

    m_separation_optimizer.reset(t_factory.clone());

    return *this;
}
