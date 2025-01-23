//
// Created by henri on 11.12.24.
//
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"

idol::Robust::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(const Robust::Description &t_description)
    : m_description(t_description) {

}

idol::Robust::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(
        const idol::Robust::ColumnAndConstraintGeneration &t_src) : m_description(t_src.m_description) {

}

idol::OptimizerFactory *idol::Robust::ColumnAndConstraintGeneration::clone() const {
    return new ColumnAndConstraintGeneration(*this);
}

idol::Robust::ColumnAndConstraintGeneration &idol::Robust::ColumnAndConstraintGeneration::with_master_optimizer(
        const idol::OptimizerFactory &t_deterministic_optimizer) {

    if (m_master_optimizer) {
        throw Exception("Master optimizer already set");
    }

    m_master_optimizer.reset(t_deterministic_optimizer.clone());

    return *this;
}

idol::Optimizer *idol::Robust::ColumnAndConstraintGeneration::operator()(const idol::Model &t_model) const {

    if (!m_master_optimizer) {
        throw Exception("Master optimizer not set");
    }

    auto* result = new Optimizers::Robust::ColumnAndConstraintGeneration(t_model,
                                                                 m_description,
                                                                 *m_master_optimizer);

    handle_default_parameters(result);

    return result;
}

