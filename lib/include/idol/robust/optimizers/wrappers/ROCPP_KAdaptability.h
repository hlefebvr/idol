//
// Created by Henri on 13/05/2026.
//

#ifndef IDOL_ROCPP_KADAPTABILITY_H
#define IDOL_ROCPP_KADAPTABILITY_H

#include "idol/bilevel/modeling/Description.h"
#include "idol/robust/modeling/Description.h"
#include "idol/general/optimizers/OptimizerFactory.h"

namespace idol::Robust::ROCPP {
    class KAdaptability;
}

class idol::Robust::ROCPP::KAdaptability : public OptimizerFactoryWithDefaultParameters<KAdaptability>  {
    const Bilevel::Description& m_bilevel_description;
    const Robust::Description& m_robust_description;
public:
    KAdaptability(const Robust::Description& t_robust_description, const Bilevel::Description& t_bilevel_description);

    [[nodiscard]] OptimizerFactory* clone() const override;
protected:
    [[nodiscard]] Optimizer* create(const Model& t_model) const override;

};

#endif //IDOL_ROCPP_KADAPTABILITY_H