//
// Created by henri on 18.09.24.
//

#ifndef IDOL_ADM_SUBPROBLEM_H
#define IDOL_ADM_SUBPROBLEM_H


#include "idol/optimizers/OptimizerFactory.h"

namespace idol::ADM {
    class SubProblem;
}

class idol::ADM::SubProblem {
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;
public:
    SubProblem() = default;

    SubProblem(const SubProblem&);
    SubProblem(SubProblem&&) = default;

    SubProblem& operator=(const SubProblem&) = delete;
    SubProblem& operator=(SubProblem&&) = default;

    SubProblem& with_optimizer(const OptimizerFactory& t_optimizer_factory);

    const OptimizerFactory& optimizer_factory() const;
};


#endif //IDOL_ADM_SUBPROBLEM_H
