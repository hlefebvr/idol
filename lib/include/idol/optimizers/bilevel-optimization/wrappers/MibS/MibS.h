//
// Created by henri on 01.02.24.
//

#ifndef IDOL_MIBS_H
#define IDOL_MIBS_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/annotations/Annotation.h"
#include "idol/modeling/constraints/Ctr.h"
#include "idol/modeling/bilevel-optimization/Description.h"

namespace idol::Bilevel {
    class MibS;
}

class idol::Bilevel::MibS : public OptimizerFactoryWithDefaultParameters<MibS> {
    Bilevel::Description m_description;
public:
    MibS(Bilevel::Description t_description);

    MibS(const MibS&) = default;
    MibS(MibS&&) noexcept = default;

    MibS& operator=(const MibS&) = delete;
    MibS& operator=(MibS&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    MibS *clone() const override;
};


#endif //IDOL_MIBS_H
