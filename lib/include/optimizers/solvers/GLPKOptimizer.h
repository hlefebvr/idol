//
// Created by henri on 23/03/23.
//

#ifndef IDOL_GLPKOPTIMIZER_H
#define IDOL_GLPKOPTIMIZER_H

#include "DefaultOptimizer.h"
#include "GLPK.h"

class GLPKOptimizer : public DefaultOptimizer<Backends::GLPK> {};

#endif //IDOL_GLPKOPTIMIZER_H
