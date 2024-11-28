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

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    const double Gamma = 3;

    // Generate parameters
    const auto filename = "/home/henri/Research/idol/tests/data/facility-location-problem/instance_F10_C20__0.txt";

    // Read instance
    const auto instance = idol::Problems::FLP::read_instance_1991_Cornuejols_et_al(filename);
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    // Make model
    auto x = Var::make_vector(env, Dim<1>(n_facilities), 0., 1., Continuous, 0., "x");
    auto y = Var::make_vector(env, Dim<2>(n_facilities, n_customers), 0., 1., Continuous, 0., "y");

    Model uncertainty_set(env);
    const auto xi = uncertainty_set.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "xi");
    uncertainty_set.add_ctr(idol_Sum(i, Range(n_facilities), xi[i]) <= Gamma);

    Model model(env);

    model.add_vector<Var, 1>(x);
    model.add_vector<Var, 2>(y);

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add(Ctr(env, idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i) * x[i]));
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add(Ctr(env, idol_Sum(i, Range(n_facilities), y[i][j]) == 1));
    }

    Robust::Description description(uncertainty_set);
    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            description.make_stage_var(y[i][j], 1);
            auto activation = model.add_ctr(y[i][j] <= 1);
            description.set_uncertain_rhs(activation, -xi[i]);
        }
    }

    model.set_obj_expr(idol_Sum(i, Range(n_facilities), instance.fixed_cost(i) * x[i] + idol_Sum(j, Range(n_customers),
                                                                                                 instance.per_unit_transportation_cost(
                                                                                                         i, j) *
                                                                                                 instance.demand(j) *
                                                                                                 y[i][j])));

    std::cout << Robust::Description::View(model, description) << std::endl;

    return 0;
}
