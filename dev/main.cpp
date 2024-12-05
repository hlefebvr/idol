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

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    // Read instance
    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("/home/henri/Research/idol/examples/robust/robust-facility-location.data.txt");
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    // Uncertainty set
    Model uncertainty_set(env);
    const double Gamma = 0;
    const auto xi = uncertainty_set.add_vars(Dim<1>(n_customers), 0, 1, Continuous, 0., "xi");
    uncertainty_set.add_ctr(idol_Sum(i, Range(n_customers), xi[i]) <= Gamma);

    // Make model
    Model model(env);
    Robust::Description description(uncertainty_set);

    auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "x");
    auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Continuous, 0., "y");

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {

        const auto c = model.add_ctr(idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i));

        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            description.set_uncertain_rhs(c, -xi[j]);
        }

    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_facilities), y[i][j]) >= 1);
    }

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            model.add_ctr(y[i][j] <= x[i]);
        }
    }

    model.set_obj_expr(idol_Sum(i, Range(n_facilities),
                                instance.fixed_cost(i) * x[i]
                                + idol_Sum(j, Range(n_customers),
                                           instance.per_unit_transportation_cost(i, j) *
                                           instance.demand(j) *
                                           y[i][j]
                                )
                       )
    );

    model.dump(std::cout);

    model.use(Gurobi());
    model.optimize();

    std::cout << "Deterministic Problem has value: " << model.get_best_obj() << std::endl;

    model.use(Robust::AffineDecisionRule(description).with_deterministic_optimizer(Gurobi()));
    model.optimize();

    std::cout << "ADR Problem has value: " << model.get_best_obj() << std::endl;

    return 0;
}
