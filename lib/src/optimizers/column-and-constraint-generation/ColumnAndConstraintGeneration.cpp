//
// Created by henri on 08.02.24.
//
#include "idol/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"

idol::Robust::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(
        const idol::Annotation<idol::Var, unsigned int> &t_lower_level_variables,
        const idol::Annotation<idol::Ctr, unsigned int> &t_lower_level_constraints,
        const idol::Model &t_uncertainty_set)
        : m_lower_level_variables(t_lower_level_variables),
          m_lower_level_constraints(t_lower_level_constraints),
          m_uncertainty_set(t_uncertainty_set)
        {

}

idol::Robust::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(
        const idol::Robust::ColumnAndConstraintGeneration &t_src)
        : m_lower_level_variables(t_src.m_lower_level_variables),
          m_lower_level_constraints(t_src.m_lower_level_constraints),
          m_uncertainty_set(t_src.m_uncertainty_set),
          m_master_optimizer(t_src.m_master_optimizer ? t_src.m_master_optimizer->clone() : nullptr),
          m_separator(t_src.m_separator ? t_src.m_separator->clone() : nullptr) {

}

idol::Optimizer *idol::Robust::ColumnAndConstraintGeneration::operator()(const idol::Model &t_model) const {

    if (!m_master_optimizer) {
        throw Exception("No master optimizer has been configured.");
    }

    if (!m_separator) {
        throw Exception("No separator has been configured.");
    }

    return new Optimizers::ColumnAndConstraintGeneration(t_model,
                                                         m_uncertainty_set,
                                                         *m_master_optimizer,
                                                         *m_separator,
                                                         m_lower_level_variables,
                                                         m_lower_level_constraints
                                                         );
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
        const idol::ColumnAndConstraintGenerationSeparator &t_separator) {

    if (m_separator) {
        throw Exception("A separator has already been configured.");
    }

    m_separator.reset(t_separator.clone());

    return *this;
}
