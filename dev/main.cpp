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
#include "idol/mixed-integer/modeling/expressions/QuadExpr.h"
#include "idol/general/utils/GenerationPattern.h"
#include "idol/mixed-integer/modeling/models/KKT.h"
#include "idol/mixed-integer/problems/facility-location-problem/FLP_Instance.h"
#include "idol/robust/modeling/Description.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/robust/optimizers/deterministic/Deterministic.h"
#include "idol/robust/optimizers/affine-decision-rule/AffineDecisionRule.h"
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/LazyCutCallback.h"
#include "idol/mixed-integer/optimizers/wrappers/Mosek/Mosek.h"
#include "idol/mixed-integer/optimizers/wrappers/Osi/Osi.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {


    Env env;

    Var u(env, 0., Inf, Continuous, 0., "u");
    Var v(env, 0., Inf, Continuous, 0., "v");
    Var w(env, 0., Inf, Continuous, 0., "w");
    Ctr c1(env, u + -2 * v + -1 * w >= 3);
    Ctr c2(env, -2 * u + v + -1 * w >= 2);
    auto objective = u + v - 2 * w;

    Model model(env);
    model.add(u);
    model.add(v);
    model.add(w);
    model.add(c1);
    model.add(c2);
    model.set_obj_expr(objective);

    model.use(GLPK());
    model.optimize();

    const double val = model.get_var_primal(u);

    return 0;
}
