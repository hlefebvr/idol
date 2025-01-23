//
// Created by henri on 06/04/23.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/problems/facility-location-problem/FLP_Instance.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestEstimate.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/callbacks/ReducedCostFixing.h"
#include "idol/robust/modeling/Description.h"
#include "idol/robust/optimizers/deterministic/Deterministic.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    // Read instance
    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("robust-facility-location.data.txt");
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    // Uncertainty set
    Model uncertainty_set(env);
    const double Gamma = 1;
    const auto xi = uncertainty_set.add_vars(Dim<1>(n_customers), 0., 1., Continuous, 0., "xi");
    uncertainty_set.add_ctr(idol_Sum(i, Range(n_customers), xi[i]) <= Gamma);

    // Make model
    Model model(env);
    Robust::Description description(uncertainty_set);

    auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "x");
    auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Continuous, 0., "y");

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {

        const auto c = model.add_ctr(idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i));

        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            description.set_uncertain_mat_coeff(c, y[i][j], 0.2 * instance.demand(j) * xi[j]);
        }

    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_facilities), y[i][j]) == 1);
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

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0; j < n_customers; ++j) {
            description.set_uncertain_obj(y[i][j], 0.2 * instance.per_unit_transportation_cost(i, j) * instance.demand(j) * xi[j]);
        }
    }

    model.use(Gurobi());
    model.optimize();
    std::cout << "Deterministic Problem has value: " << model.get_best_obj() << std::endl;

    /*
     * Alternatively, you could also do
     * const auto deterministic_model = Robust::Deterministic::make_model(model, description);
     * std::cout << Robust::Description::View(model, description) << std::endl;
     */

    model.use(
                Robust::Deterministic(description)
                    .with_deterministic_optimizer(Gurobi().with_logs(false))
            );
    model.optimize();
    std::cout << "Robust Problem has value: " << model.get_best_obj() << std::endl;

    return 0;
}
