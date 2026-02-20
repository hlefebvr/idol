//
// Created by henri on 12/09/22.
//

#ifndef OPTIMIZE_OPTIMIZERS_H
#define OPTIMIZE_OPTIMIZERS_H

// Branch-and-Bound
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/UniformlyRandom.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/FirstInfeasibleFound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/StrongBranching.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/LeastInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BreadthFirst.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/DepthFirst.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestEstimate.h"

// Dantzig-Wolfe decomposition
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/NoStabilization.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/Neame.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/Wentges.h"

// Wrappers
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

#endif //OPTIMIZE_OPTIMIZERS_H
