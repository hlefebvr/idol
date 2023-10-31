//
// Created by henri on 31.10.23.
//
#include "idol/optimizers/dantzig-wolfe/DantzigWolfeSubProblem.h"

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
idol::DantzigWolfe::SubProblem::add_optimizer(const OptimizerFactory &t_optimizer) {

    m_phase_optimizers.emplace_back(t_optimizer.clone());

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
      m_max_column_per_pricing(t_src.m_max_column_per_pricing)
{

    for (auto& ptr_optimizer : t_src.m_phase_optimizers) {
        m_phase_optimizers.emplace_back(ptr_optimizer->clone());
    }

}

double idol::DantzigWolfe::SubProblem::lower_multiplicity() const {
    return m_lower_multiplicity.has_value() ? m_lower_multiplicity.value() : 1.;
}

double idol::DantzigWolfe::SubProblem::upper_multiplicity() const {
    return m_upper_multiplicity.has_value() ? m_upper_multiplicity.value() : 1.;
}
