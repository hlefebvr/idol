//
// Created by henri on 12.04.25.
//

#ifndef IDOL_SCENARIOBASEDKADAPTABILITYPROBLEMSOLVER_H
#define IDOL_SCENARIOBASEDKADAPTABILITYPROBLEMSOLVER_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/robust/modeling/Description.h"

namespace idol::Robust {
    class ScenarioBasedKAdaptabilityProblemSolver;
}

class idol::Robust::ScenarioBasedKAdaptabilityProblemSolver : public OptimizerFactoryWithDefaultParameters<ScenarioBasedKAdaptabilityProblemSolver> {
    unsigned int m_K;
    std::unique_ptr<OptimizerFactory> m_main_optimizer;
    std::unique_ptr<OptimizerFactory> m_separation_optimizer;
    const Bilevel::Description& m_bilevel_description;
    const Robust::Description& m_robust_description;
public:
    ScenarioBasedKAdaptabilityProblemSolver(const Bilevel::Description& t_bilevel_description,
                                            const Robust::Description& t_robust_description,
                                            unsigned int t_K);

    ScenarioBasedKAdaptabilityProblemSolver(const ScenarioBasedKAdaptabilityProblemSolver& t_src);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] ScenarioBasedKAdaptabilityProblemSolver *clone() const override;

    ScenarioBasedKAdaptabilityProblemSolver& with_optimizer(const OptimizerFactory& t_optimizer);

    ScenarioBasedKAdaptabilityProblemSolver& with_separation_optimizer(const OptimizerFactory& t_optimizer);
};

#endif //IDOL_SCENARIOBASEDKADAPTABILITYPROBLEMSOLVER_H
