//
// Created by henri on 12/09/22.
//

#ifndef OPTIMIZE_OPTIMIZERS_H
#define OPTIMIZE_OPTIMIZERS_H

// Branch-and-Bound
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/branching-rules/factories/UniformlyRandom.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/branching-rules/factories/FirstInfeasibleFound.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/branching-rules/factories/StrongBranching.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/branching-rules/factories/LeastInfeasible.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/node-selection-rules/factories/BreadthFirst.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/node-selection-rules/factories/DepthFirst.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/node-selection-rules/factories/BestEstimate.h"

// Dantzig-Wolfe decomposition
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/stabilization/NoStabilization.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/stabilization/Neame.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/stabilization/Wentges.h"

// Wrappers
#include "idol/optimizers/mixed-integer-optimization/wrappers/GLPK/GLPK.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/HiGHS/HiGHS.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Mosek/Mosek.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h"

#endif //OPTIMIZE_OPTIMIZERS_H
