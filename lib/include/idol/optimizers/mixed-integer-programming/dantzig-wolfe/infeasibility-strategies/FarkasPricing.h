//
// Created by henri on 31.10.23.
//

#ifndef IDOL_FARKASPRICING_H
#define IDOL_FARKASPRICING_H

#include "DantzigWolfeInfeasibilityStrategy.h"

namespace idol::DantzigWolfe {
    class FarkasPricing;
}

class idol::DantzigWolfe::FarkasPricing : public idol::DantzigWolfe::InfeasibilityStrategyFactory {
public:

    class Strategy : public InfeasibilityStrategyFactory::Strategy {
    public:
        void execute(Optimizers::DantzigWolfeDecomposition &t_parent) override;
    };

    InfeasibilityStrategyFactory::Strategy *operator()() const override {
        return new Strategy();
    }

    [[nodiscard]] FarkasPricing *clone() const override {
        return new FarkasPricing(*this);
    }


};

#endif //IDOL_FARKASPRICING_H
