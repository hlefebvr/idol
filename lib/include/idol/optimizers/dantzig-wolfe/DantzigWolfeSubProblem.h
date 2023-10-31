//
// Created by henri on 31.10.23.
//

#ifndef IDOL_DANTZIGWOLFESUBPROBLEM_H
#define IDOL_DANTZIGWOLFESUBPROBLEM_H

#include <list>
#include <memory>
#include <optional>
#include "idol/optimizers/OptimizerFactory.h"
#include "idol/containers/IteratorForward.h"

namespace idol::DantzigWolfe {
    class SubProblem;
}

class idol::DantzigWolfe::SubProblem {
    std::optional<double> m_lower_multiplicity;
    std::optional<double> m_upper_multiplicity;

    std::list<std::unique_ptr<OptimizerFactory>> m_phase_optimizers;

    std::optional<unsigned int> m_max_column_per_pricing;
public:
    SubProblem() = default;

    SubProblem(const SubProblem& t_src);
    SubProblem(SubProblem&& t_src) = default;

    SubProblem& operator=(const SubProblem&) = delete;
    SubProblem& operator=(SubProblem&&) = default;

    SubProblem& with_multiplicities(double t_lower, double t_upper);

    SubProblem& with_lower_multiplicity(double t_lower);

    SubProblem& with_upper_multiplicity(double t_upper);

    SubProblem& add_optimizer(const OptimizerFactory &t_optimizer);

    SubProblem& with_max_column_per_pricing(unsigned int t_n_columns);

    double lower_multiplicity() const;

    double upper_multiplicity() const;

    using PhaseId = std::list<std::unique_ptr<OptimizerFactory>>::const_iterator;

    auto phases() const { return ConstIteratorForward(m_phase_optimizers); }
};

#endif //IDOL_DANTZIGWOLFESUBPROBLEM_H
