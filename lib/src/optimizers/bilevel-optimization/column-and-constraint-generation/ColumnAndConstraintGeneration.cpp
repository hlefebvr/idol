//
// Created by henri on 04.03.24.
//
#include <utility>
#include "idol/optimizers/bilevel-optimization/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/optimizers/bilevel-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"

idol::Bilevel::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(
        const idol::Annotation<idol::Var, unsigned int> &t_lower_level_variables,
        const idol::Annotation<idol::Ctr, unsigned int> &t_lower_level_constraints,
        idol::Ctr t_lower_level_objective)
        : m_lower_level_objective(std::move(t_lower_level_objective)),
            m_lower_level_variables(t_lower_level_variables),
            m_lower_level_constraints(t_lower_level_constraints) {

}

idol::Bilevel::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(
        const idol::Bilevel::ColumnAndConstraintGeneration &t_src)
            :
            OptimizerFactoryWithDefaultParameters<ColumnAndConstraintGeneration>(t_src),
            m_lower_level_objective(t_src.m_lower_level_objective),
            m_lower_level_variables(t_src.m_lower_level_variables),
            m_lower_level_constraints(t_src.m_lower_level_constraints),
            m_master_optimizer(t_src.m_master_optimizer ? t_src.m_master_optimizer->clone() : nullptr),
            m_lower_level_optimizer(t_src.m_lower_level_optimizer ? t_src.m_lower_level_optimizer->clone() : nullptr) {

}

idol::Bilevel::ColumnAndConstraintGeneration *idol::Bilevel::ColumnAndConstraintGeneration::clone() const {
    return new ColumnAndConstraintGeneration(*this);
}

idol::Bilevel::ColumnAndConstraintGeneration &
idol::Bilevel::ColumnAndConstraintGeneration::with_master_optimizer(const idol::OptimizerFactory &t_optimizer) {

    if (m_master_optimizer) {
        throw Exception("A master optimizer for ColumnAndConstraintGeneration has already been given.");
    }

    m_master_optimizer.reset(t_optimizer.clone());

    return *this;

}

idol::Bilevel::ColumnAndConstraintGeneration &
idol::Bilevel::ColumnAndConstraintGeneration::with_lower_level_optimizer(const idol::OptimizerFactory &t_optimizer) {

    if (m_lower_level_optimizer) {
        throw Exception("A lower level optimizer for ColumnAndConstraintGeneration has already been given.");
    }

    m_lower_level_optimizer.reset(t_optimizer.clone());

    return *this;

}

idol::Optimizer *idol::Bilevel::ColumnAndConstraintGeneration::operator()(const idol::Model &t_model) const {

    if (!m_master_optimizer) {
        throw Exception("A master optimizer for ColumnAndConstraintGeneration has not been given.");
    }

    if (!m_lower_level_optimizer) {
        throw Exception("A lower level optimizer for ColumnAndConstraintGeneration has not been given.");
    }

    return new Optimizers::Bilevel::ColumnAndConstraintGeneration(
                t_model,
                m_lower_level_variables,
                m_lower_level_constraints,
                m_lower_level_objective,
                *m_master_optimizer,
                *m_lower_level_optimizer
            );
}
