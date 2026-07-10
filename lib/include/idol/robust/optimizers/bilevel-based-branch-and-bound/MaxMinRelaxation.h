//
// Created by Henri on 01/06/2026.
//

#ifndef IDOL_ROBUST_BBBB_H
#define IDOL_ROBUST_BBBB_H

#include <idol/general/optimizers/OptimizerFactory.h>
#include <idol/robust/modeling/Description.h>
#include <idol/bilevel/modeling/Description.h>

namespace idol::Robust {
    class MaxMinRelaxation;
}

class idol::Robust::MaxMinRelaxation : public OptimizerFactoryWithDefaultParameters<MaxMinRelaxation> {
    const Description& m_description;
    const Bilevel::Description& m_bilevel_description;
    std::unique_ptr<OptimizerFactory> m_master_optimizer_factory;
    std::unique_ptr<OptimizerFactory> m_deterministic_optimizer_factory;
    std::optional<bool> m_use_indicator;
    std::list<PrimalPoint> m_initial_scenarios;
protected:
    [[nodiscard]] Optimizer* create(const Model& t_model) const override;
    MaxMinRelaxation(const MaxMinRelaxation& t_src);
public:
    MaxMinRelaxation(const Robust::Description& t_description, const Bilevel::Description& t_bilevel_description);

    [[nodiscard]] OptimizerFactory* clone() const override;

    MaxMinRelaxation& with_master_optimizer(const OptimizerFactory& t_optimizer);
    MaxMinRelaxation& with_deterministic_optimizer(const OptimizerFactory& t_optimizer);
    MaxMinRelaxation& with_indicator(bool t_value);
    MaxMinRelaxation& add_initial_scenario(PrimalPoint t_scenario);
};

#endif //IDOL_ROBUST_BBBB_H