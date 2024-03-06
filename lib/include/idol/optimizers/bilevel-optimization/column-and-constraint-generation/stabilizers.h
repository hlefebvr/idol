//
// Created by henri on 06.03.24.
//

#ifndef IDOL_STABILIZERS_H
#define IDOL_STABILIZERS_H

#include "idol/optimizers/robust-optimization/column-and-constraint-generation/stabilizers/NoStabilization.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/stabilizers/TrustRegion.h"

namespace idol::Bilevel {
    using CCGStabilizer = Robust::CCGStabilizer;
    namespace CCGStabilizers = idol::Robust::CCGStabilizers;
}

#endif //IDOL_STABILIZERS_H
