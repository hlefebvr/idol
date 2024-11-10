#include <iostream>
#include "idol/general/utils/SparseVector.h"
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/DantzigWolfeInfeasibilityStrategy.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/Neame.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/IntegerMaster.h"
#include "idol/general/utils/SparseVectorTSL.h"
#include <gurobi_c++.h>

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;
    Model model(env);

    const auto x = model.add_vars(Dim<1>(10), 0, 1, Continuous, 0., "x");

    SparseVectorTSL<Var, double> vec;

    vec.set(x[0], 0);
    vec.set(x[1], 1);
    vec.set(x[2], 2);

    vec += vec;

    SparseVectorTSL<Var, double> vec2;
    vec2.set(x[1], 2);

    vec -= vec2;

    for (const auto& [var, val] : vec) {
        std::cout << var << " = " << val << std::endl;
    }

    return 0;
}
