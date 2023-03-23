//
// Created by henri on 23/03/23.
//

#ifndef IDOL_GUROBIOPTIMIZER_H
#define IDOL_GUROBIOPTIMIZER_H

#ifdef IDOL_USE_GUROBI

#include "DefaultOptimizer.h"
#include "Gurobi.h"

class GurobiOptimizer : public DefaultOptimizer<Backends::Gurobi> {};

#endif

#endif //IDOL_GUROBIOPTIMIZER_H
