//
// Created by Henri on 21/01/2026.
//

#ifndef IDOL_CCG_OPTIMALITYSEPARATION_H
#define IDOL_CCG_OPTIMALITYSEPARATION_H
#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/Separation.h"

namespace idol::Robust::CCG {
    class OptimalitySeparation;
}

class idol::Robust::CCG::OptimalitySeparation : public idol::Robust::CCG::Separation {
    std::unique_ptr<OptimizerFactory> m_bilevel_optimizer;

    OptimalitySeparation(const OptimalitySeparation& t_src);
public:
    OptimalitySeparation() = default;

    void operator()() override;
    [[nodiscard]] Separation* clone() const override { return new OptimalitySeparation(*this); }

    OptimalitySeparation& with_bilevel_optimizer(const OptimizerFactory& t_optimizer);
};

#endif //IDOL_CCG_OPTIMALITYSEPARATION_H