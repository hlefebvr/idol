//
// Created by henri on 31.10.23.
//
#include "idol/optimizers/column-generation/DantzigWolfeSubProblem.h"

idol::DantzigWolfe::SubProblem &idol::DantzigWolfe::SubProblem::with_multiplicities(double t_lower, double t_upper) {

    with_lower_multiplicity(t_lower);
    with_upper_multiplicity(t_upper);

    return *this;
}

idol::DantzigWolfe::SubProblem &
idol::DantzigWolfe::SubProblem::with_lower_multiplicity(double t_lower) {

    if (m_lower_multiplicity) {
        throw Exception("A lower multiplicity has already been configured.");
    }

    m_lower_multiplicity = t_lower;

    return *this;
}

idol::DantzigWolfe::SubProblem &
idol::DantzigWolfe::SubProblem::with_upper_multiplicity(double t_upper) {

    if (m_upper_multiplicity) {
        throw Exception("A upper multiplicity has already been configured.");
    }

    m_upper_multiplicity = t_upper;

    return *this;
}

idol::DantzigWolfe::SubProblem &
idol::DantzigWolfe::SubProblem::add_heuristic_optimizer(const idol::OptimizerFactory &t_heuristic_optimizer) {

    m_heuristic_optimizer_factories.emplace_back(t_heuristic_optimizer.clone());

    return *this;
}

idol::DantzigWolfe::SubProblem &
idol::DantzigWolfe::SubProblem::with_exact_optimizer(const OptimizerFactory &t_exact_optimizer) {

    if (m_exact_optimizer_factory) {
        throw Exception("An exact optimizer factory has already been configured.");
    }

    m_exact_optimizer_factory.reset(t_exact_optimizer.clone());

    return *this;
}

idol::DantzigWolfe::SubProblem &idol::DantzigWolfe::SubProblem::with_max_column_per_pricing(unsigned int t_n_columns) {

    if (m_max_column_per_pricing) {
        throw Exception("A maximum number of column per pricing operation has already been configure.");
    }

    m_max_column_per_pricing = t_n_columns;

    return *this;
}

idol::DantzigWolfe::SubProblem::SubProblem(const idol::DantzigWolfe::SubProblem &t_src)
    : m_lower_multiplicity(t_src.m_lower_multiplicity),
      m_upper_multiplicity(t_src.m_upper_multiplicity),
      m_exact_optimizer_factory(t_src.m_exact_optimizer_factory ? t_src.m_exact_optimizer_factory->clone() : nullptr),
      m_max_column_per_pricing(t_src.m_max_column_per_pricing)
{

    for (auto& ptr_optimizer : t_src.m_heuristic_optimizer_factories) {
        m_heuristic_optimizer_factories.emplace_back(ptr_optimizer->clone());
    }

}

double idol::DantzigWolfe::SubProblem::lower_multiplicity() const {
    return m_lower_multiplicity.has_value() ? m_lower_multiplicity.value() : 1.;
}

double idol::DantzigWolfe::SubProblem::upper_multiplicity() const {
    return m_upper_multiplicity.has_value() ? m_upper_multiplicity.value() : 1.;
}
