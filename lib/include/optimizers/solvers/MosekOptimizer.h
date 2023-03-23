//
// Created by henri on 23/03/23.
//

#ifndef IDOL_MOSEKOPTIMIZER_H
#define IDOL_MOSEKOPTIMIZER_H

#ifdef IDOL_USE_MOSEK

#include "DefaultOptimizer.h"
#include "Mosek.h"

class MosekOptimizer : public DefaultOptimizer<Backends::Mosek> {};

#endif

#endif //IDOL_MOSEKOPTIMIZER_H
