//
// Created by henri on 06.03.24.
//

#ifndef IDOL_TRUSTREGION_H
#define IDOL_TRUSTREGION_H

#include "CCGStabilizer.h"

namespace idol::Robust::CCGStabilizers {
    class TrustRegion;
}

class idol::Robust::CCGStabilizers::TrustRegion : public idol::Robust::CCGStabilizer {
public:
    class Strategy : public CCGStabilizer::Strategy {

    };

    Strategy *operator()() const override;

    CCGStabilizer *clone() const override;
};

#endif //IDOL_TRUSTREGION_H
