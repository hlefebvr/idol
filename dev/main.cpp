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

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    // Read instance
    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("/home/henri/Research/idol/examples/robust/robust-facility-location.data.txt");
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    // Uncertainty set
    Model uncertainty_set(env);
    const double Gamma = 5;
    const auto xi = uncertainty_set.add_vars(Dim<1>(n_customers), 0, 1, Continuous, 0., "xi");
    uncertainty_set.add_ctr(idol_Sum(i, Range(n_customers), xi[i]) <= Gamma);

    // Make model
    Model model(env);

    Robust::Description robust_description(uncertainty_set);
    Bilevel::Description bilevel_description(env);

    const auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "x");
    const auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., Inf, Continuous, 0., "y");

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        const auto c = model.add_ctr(idol_Sum(j, Range(n_customers), y[i][j]) <= instance.capacity(i) * x[i], "capacity_" + std::to_string(i));
        bilevel_description.make_lower_level(c);
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        const auto c = model.add_ctr(idol_Sum(i, Range(n_facilities), y[i][j]) >= instance.demand(j), "demand_" + std::to_string(j));
        bilevel_description.make_lower_level(c);
        robust_description.set_uncertain_rhs(c, 0.2 * instance.demand(j) * xi[j]);
    }

    // Set second-stage variables
    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            bilevel_description.make_lower_level(y[i][j]);
            model.set_var_ub(y[i][j], 1.2 * instance.demand(j));
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

    model.use(Gurobi());
    model.optimize();

    std::cout << "Deterministic Problem has value: " << model.get_best_obj() << std::endl;

    const auto bilevel_optimizer =
            Bilevel::KKT()
                .with_single_level_optimizer(
                        Gurobi()
                            .with_presolve(false)
                            .with_logs(false)
                )
    ;

    model.use(
            Robust::ColumnAndConstraintGeneration(robust_description, bilevel_description)
                    .with_master_optimizer(Gurobi())
                    //.with_initial_scenario_by_minimization(Gurobi())
                    //.with_initial_scenario_by_maximization(Gurobi())
                    .add_feasibility_separation_optimizer(bilevel_optimizer)
                    .add_optimality_separation_optimizer(bilevel_optimizer)
                    .with_logs(true)
    );
    model.optimize();

    std::cout << "Status is " << model.get_status() << std::endl;
    std::cout << "Two-stage Robust Problem has value: " << model.get_best_obj() << std::endl;

    return 0;
}
