#include "idol/modeling.h"
#include "idol/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"
#include "idol/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/optimizers/callbacks/IntegerMaster.h"
#include "idol/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/optimizers/dantzig-wolfe/stabilization/Neame.h"
#include "idol/optimizers/dantzig-wolfe/stabilization/Wentges.h"
#include "idol/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h"
#include "idol/problems/facility-location-problem/FLP_Instance.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestEstimate.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/BracnhingWithPriority.h"
#include "idol/optimizers/wrappers/Mosek/Mosek.h"

using namespace idol;

int main(int t_argc, char** t_argv) {

    Env env;

    Model model(env);

    model.use(Mosek());

    return 0;
}
