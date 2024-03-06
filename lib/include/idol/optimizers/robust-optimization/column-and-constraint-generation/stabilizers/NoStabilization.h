//
// Created by henri on 06.03.24.
//

#ifndef IDOL_NOSTABILIZATION_H
#define IDOL_NOSTABILIZATION_H

#include "CCGStabilizer.h"

namespace idol::Robust::CCGStabilizers {
    class NoStabilization;
}

class idol::Robust::CCGStabilizers::NoStabilization : public idol::Robust::CCGStabilizer {
public:
    NoStabilization() = default;

    NoStabilization(const NoStabilization&) = default;
    NoStabilization(NoStabilization&&) = default;

    NoStabilization& operator=(const NoStabilization&) = default;
    NoStabilization& operator=(NoStabilization&&) = default;

    class Strategy : public CCGStabilizer::Strategy {
    };

    CCGStabilizer::Strategy *operator()() const override;

    idol::Robust::CCGStabilizer *clone() const override;
};

#endif //IDOL_NOSTABILIZATION_H
