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

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;
    Model primal(env);

    const auto x = primal.add_vars(Dim<1>(10), 0, 1, Continuous, -1, "x");
    primal.add_ctr(idol_Sum(i, Range(10), i * x[i]) <= 5);

    Model dual(env);
    Model strong_duality(env);
    Model kkt(env);

    KKT dualizer(primal, primal.get_obj_expr());
    dualizer.add_dual(dual);
    dualizer.add_strong_duality_reformulation(strong_duality);
    dualizer.add_kkt_reformulation(kkt);

    std::cout << primal << std::endl;
    std::cout << dual << std::endl;
    std::cout << strong_duality << std::endl;
    std::cout << kkt << std::endl;

    primal.use(Gurobi());
    dual.use(Gurobi());
    strong_duality.use(Gurobi());
    kkt.use(Gurobi());

    primal.optimize();
    dual.optimize();
    strong_duality.optimize();
    kkt.optimize();

    std::cout << "Primal solution: " << primal.get_best_obj() << std::endl;
    std::cout << "Dual solution: " << dual.get_best_obj() << std::endl;
    std::cout << "Strong duality solution: " << evaluate(dualizer.get_dual_obj_expr(), save_primal(strong_duality)) << std::endl;
    std::cout << "KKT solution: " << evaluate(dualizer.get_dual_obj_expr(), save_primal(kkt)) << std::endl;

    return 0;
}
