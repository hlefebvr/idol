//
// Created by Henri on 03/02/2026.
//

#ifndef IDOL_NESTEDBRANCHANDCUT_H
#define IDOL_NESTEDBRANCHANDCUT_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/robust/modeling/Description.h"
#include "idol/bilevel/modeling/Description.h"

namespace idol::Robust {
    class BilevelBasedBranchAndBound;
}

class idol::Robust::BilevelBasedBranchAndBound : public OptimizerFactoryWithDefaultParameters<BilevelBasedBranchAndBound> {
    const Bilevel::Description& m_bilevel_description;
    const Robust::Description& m_robust_description;

    std::unique_ptr<OptimizerFactory> m_optimality_bilevel_optimizer;
    std::unique_ptr<OptimizerFactory> m_feasibility_bilevel_optimizer;
    std::optional<bool> m_with_first_stage_relaxation;
public:
    BilevelBasedBranchAndBound(const Robust::Description& t_robust_description,
                       const Bilevel::Description& t_bilevel_description);

    BilevelBasedBranchAndBound(const BilevelBasedBranchAndBound& t_src);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override { return new BilevelBasedBranchAndBound(*this); }

    BilevelBasedBranchAndBound& with_optimality_bilevel_optimizer(const OptimizerFactory& t_factory);

    BilevelBasedBranchAndBound& with_feasibility_bilevel_optimizer(const OptimizerFactory& t_factory);

    BilevelBasedBranchAndBound& with_first_stage_relaxation(bool t_value);

};

#endif //IDOL_NESTEDBRANCHANDCUT_H