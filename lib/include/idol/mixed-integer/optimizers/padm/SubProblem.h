//
// Created by henri on 18.09.24.
//

#ifndef IDOL_ADM_SUBPROBLEM_H
#define IDOL_ADM_SUBPROBLEM_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/general/utils/Point.h"
#include <memory>

namespace idol::ADM {
    class SubProblem;
}

class idol::ADM::SubProblem {
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;
    std::optional<PrimalPoint> m_initial_point;
public:
    SubProblem() = default;

    SubProblem(const SubProblem&);
    SubProblem(SubProblem&&) = default;

    SubProblem& operator=(const SubProblem&) = delete;
    SubProblem& operator=(SubProblem&&) = default;

    SubProblem& with_optimizer(const OptimizerFactory& t_optimizer_factory);

    SubProblem& with_initial_point(const PrimalPoint& t_initial_point, bool t_force = false);

    const OptimizerFactory& optimizer_factory() const;

    PrimalPoint initial_point() const;
};


#endif //IDOL_ADM_SUBPROBLEM_H
