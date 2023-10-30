//
// Created by henri on 23/03/23.
//

#ifndef IDOL_MINKNAP_H
#define IDOL_MINKNAP_H

#include "idol/optimizers/OptimizerFactory.h"

namespace idol {
    class MinKnap;
}

class idol::MinKnap : public OptimizerFactoryWithDefaultParameters<MinKnap> {
    MinKnap(const MinKnap&) = default;
public:
    MinKnap() = default;
    MinKnap(MinKnap&&) noexcept = default;

    MinKnap& operator=(const MinKnap&) = delete;
    MinKnap& operator=(MinKnap&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] MinKnap *clone() const override;
};

#endif //IDOL_MINKNAP_H
