//
// Created by henri on 06/04/23.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/problems/facility-location-problem/FLP_Instance.h"
#include "idol/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/VariableBranching.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/UniformlyRandom.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/StrongBranching.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/strong-branching/StrongBranchingPhase.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/optimizers/branch-and-bound/nodes/NodeVarInfo.h"

int main(int t_argc, const char** t_argv) {

    using namespace idol;

    Env env;

    // Read instance
    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("instance.txt");
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    // Make model

    Model model(env);

    auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, "x");
    auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Continuous, "y");

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add_ctr(idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i) * x[i]);
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_facilities), y[i][j]) == 1);
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

    // Set backend options
    model.use(
            BranchAndBound()
                    .with_node_optimizer(Gurobi::ContinuousRelaxation())
                    .with_branching_rule(
                            PseudoCost()
                    )
                .with_node_selection_rule(BestBound())
                .with_log_level(Trace, Blue)
    );

    model.optimize();

    std::cout << save_primal(model) << std::endl;

    return 0;
}
