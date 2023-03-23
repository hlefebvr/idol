//
// Created by henri on 23/03/23.
//

#ifndef IDOL_GLPKOPTIMIZER_H
#define IDOL_GLPKOPTIMIZER_H

#ifdef IDOL_USE_GLPK

#include "DefaultOptimizer.h"
#include "GLPK.h"

class GLPKOptimizer : public DefaultOptimizer<Backends::GLPK> {};

#endif

#endif //IDOL_GLPKOPTIMIZER_H
