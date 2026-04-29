//
// Created by Henri on 29/04/2026.
//

#ifndef IDOL_YASOL_H
#define IDOL_YASOL_H

#include "idol/bilevel/modeling/Description.h"
#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/robust/modeling/Description.h"

namespace idol::Robust {
    class Yasol;
}

class idol::Robust::Yasol : public OptimizerFactoryWithDefaultParameters<Yasol> {
    const Robust::Description& m_robust_description;
    const Bilevel::Description* m_bilevel_description = nullptr;
protected:
    [[nodiscard]] Optimizer* create(const Model& t_model) const override;
public:
    Yasol(const Robust::Description& t_robust_description, const Bilevel::Description& t_bilevel_description);

    Yasol(const Robust::Description& t_robust_description);

    Yasol(const Yasol&) = default;

    [[nodiscard]] OptimizerFactory* clone() const override;
};

#endif //IDOL_YASOL_H