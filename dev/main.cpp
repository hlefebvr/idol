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
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    // Create model
    Model model(env);
    const auto y = model.add_var(0, Inf, Integer, 2, "y");
    const auto z = model.add_var(0, Inf, Continuous, 1, "z");

    // Create separation problem
    Model separation(env);
    const auto lambda = separation.add_vars(Dim<1>(2), 0, Inf, Continuous, 0, "lambda");
    separation.add_ctr(lambda[0] + 2 * lambda[1] <= 2);
    separation.add_ctr(2 * lambda[0] - lambda[1] <= 3);
    const auto benders_cut = z - (!lambda[0] * 3 - !lambda[0] * y + !lambda[1] * 4 - !lambda[1] * 3 * y);

    auto cb = LazyCutCallback(separation, benders_cut, GreaterOrEqual);
    cb.with_separation_optimizer(Cplex());

    auto cplex = Cplex();
    cplex.add_callback(cb);
    cplex.with_lazy_cut(true);
    cplex.with_logs(true);

    model.use(cplex);
    model.optimize();

    std::cout << "Objective value = " << model.get_best_obj() << std::endl;
    std::cout << "Solution:\n";
    std::cout << save_primal(model) << std::endl;

    return 0;
}
