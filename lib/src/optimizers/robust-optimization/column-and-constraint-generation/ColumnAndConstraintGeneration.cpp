//
// Created by henri on 08.02.24.
//
#include <utility>

#include "idol/optimizers/robust-optimization/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/stabilizers/NoStabilization.h"

idol::Robust::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(
        Robust::StageDescription t_stage_description,
        const idol::Model &t_uncertainty_set)
        : OptimizerFactoryWithDefaultParameters<ColumnAndConstraintGeneration>(),
          m_stage_description(std::move(t_stage_description)),
          m_uncertainty_set(t_uncertainty_set)
        {

}

idol::Robust::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(
        const idol::Robust::ColumnAndConstraintGeneration &t_src)
        :
          OptimizerFactoryWithDefaultParameters<ColumnAndConstraintGeneration>(t_src),
          m_stage_description(t_src.m_stage_description),
          m_uncertainty_set(t_src.m_uncertainty_set),
          m_master_optimizer(t_src.m_master_optimizer ? t_src.m_master_optimizer->clone() : nullptr),
          m_separator(t_src.m_separator ? t_src.m_separator->clone() : nullptr),
          m_stabilizer(t_src.m_stabilizer ? t_src.m_stabilizer->clone() : nullptr),
          m_complete_recourse(t_src.m_complete_recourse) {

}

idol::Optimizer *idol::Robust::ColumnAndConstraintGeneration::operator()(const idol::Model &t_model) const {

    if (!m_master_optimizer) {
        throw Exception("No master optimizer has been configured.");
    }

    if (!m_separator) {
        throw Exception("No separator has been configured.");
    }

    auto *result = new Optimizers::Robust::ColumnAndConstraintGeneration(t_model,
                                                         m_uncertainty_set,
                                                         *m_master_optimizer,
                                                         *m_separator,
                                                         m_stabilizer ? *m_stabilizer : (const CCGStabilizer&) CCGStabilizers::NoStabilization(),
                                                         m_stage_description,
                                                         m_complete_recourse.has_value() && m_complete_recourse.value()
                                                         );

    this->handle_default_parameters(result);

    return result;
}

idol::Robust::ColumnAndConstraintGeneration *idol::Robust::ColumnAndConstraintGeneration::clone() const {
    return new ColumnAndConstraintGeneration(*this);
}

idol::Robust::ColumnAndConstraintGeneration &
idol::Robust::ColumnAndConstraintGeneration::with_master_optimizer(const idol::OptimizerFactory &t_optimizer) {

    if (m_master_optimizer) {
        throw Exception("A master optimizer has already been configured.");
    }

    m_master_optimizer.reset(t_optimizer.clone());

    return *this;
}

idol::Robust::ColumnAndConstraintGeneration &idol::Robust::ColumnAndConstraintGeneration::with_separator(
        const idol::Robust::CCGSeparator &t_separator) {

    if (m_separator) {
        throw Exception("A separator has already been configured.");
    }

    m_separator.reset(t_separator.clone());

    return *this;
}

idol::Robust::ColumnAndConstraintGeneration &
idol::Robust::ColumnAndConstraintGeneration::with_complete_recourse(bool t_value) {

    if (m_complete_recourse.has_value()) {
        throw Exception("Complete recourse has already been configured.");
    }

    m_complete_recourse = t_value;

    return *this;
}

idol::Robust::ColumnAndConstraintGeneration &
idol::Robust::ColumnAndConstraintGeneration::with_stabilization(const idol::Robust::CCGStabilizer &t_stabilizer) {

    if (m_stabilizer) {
        throw Exception("A stabilizer has already been configured.");
    }

    m_stabilizer.reset(t_stabilizer.clone());

    return *this;
}
