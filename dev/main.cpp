#include "idol/modeling.h"
#include "idol/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/optimizers/branch-and-bound/nodes/NodeVarInfo.h"
#include "idol/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/optimizers/column-generation/IntegerMaster.h"
#include "idol/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/optimizers/wrappers/Gurobi/Gurobi.h"

using namespace idol;

int main(int t_argc, char** t_argv) {

    using Solver = Gurobi;

    Env env;

    Model model(env);

    model.use(Solver());

    auto x = model.add_vars(Dim<1>(2), 0, 1, Binary, "x");

    model.remove(x[0]);

    model.write("model.lp");

    std::unique_ptr<Model> copy(model.clone());

    copy->use(Solver());

    copy->write("copy.lp");

    return 0;
}
