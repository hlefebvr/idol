//
// Created by Henri on 26/05/2026.
//

#ifndef IDOL_CONVEXIFICATION_H
#define IDOL_CONVEXIFICATION_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include <memory>

namespace idol::Bilevel {
    class Description;
}

namespace idol::Robust {
    class Description;
    class Convexification;
}

class idol::Robust::Convexification : public idol::OptimizerFactoryWithDefaultParameters<Convexification> {
    const Robust::Description& m_robust_description;
    const Bilevel::Description& m_bilevel_description;
    std::unique_ptr<OptimizerFactory> m_master_problem_optimizer;
    std::unique_ptr<OptimizerFactory> m_sub_problem_level_optimizer;
    std::unique_ptr<OptimizerFactory> m_integer_master_heuristic_optimizer;
protected:
    [[nodiscard]] Optimizer *create(const Model &t_model) const override;
public:
    explicit Convexification(const Robust::Description& t_robust_description, const Bilevel::Description& t_bilevel_description);

    Convexification(const Convexification& t_src);

    [[nodiscard]] OptimizerFactory *clone() const override;

    Convexification& with_master_optimizer(const OptimizerFactory& t_optimizer);

    Convexification& with_sub_problem_optimizer(const OptimizerFactory& t_optimizer);

    Convexification& with_integer_master_heuristic_optimizer(const OptimizerFactory& t_optimizer);

    Convexification& operator+=(const OptimizerFactory& t_optimizer) { return with_master_optimizer(t_optimizer); }
};


#endif //IDOL_CONVEXIFICATION_H