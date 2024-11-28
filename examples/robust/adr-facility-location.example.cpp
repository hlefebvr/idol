//
// Created by henri on 28.11.24.
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
#include "idol/robust/optimizers/affine-decision-rule/AffineDecisionRule.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    // Read instance
    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("adr-facility-location.data.txt");
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    // Uncertainty set
    Model uncertainty_set(env);
    const double Gamma = .2;
    const auto xi = uncertainty_set.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Continuous, 0., "xi");
    uncertainty_set.add_ctr(idol_Sum(i, Range(n_facilities), idol_Sum(j, Range(n_customers), xi[i][j])) <= Gamma);

    // Make model
    Model model(env);
    Robust::Description description(uncertainty_set);

    auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "x");
    auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Continuous, 0., "y");

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add_ctr(idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i));
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

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0; j < n_customers; ++j) {
            const auto c = model.add_ctr(y[i][j] <= 1);
            description.set_uncertain_rhs(c, -xi[i][j]); // models y_ij <= 1 - xi_j
            description.set_stage(y[i][j], 1); // models y_ij as a second-stage variable
        }
    }

    /*
     * const auto adr_result = Robust::AffineDecisionRule::make_model(model, description);
     * std::cout << Robust::Description::View(adr_result.model, description) << std::endl;
     */

    const auto adr_result = Robust::AffineDecisionRule::make_model(model, description);
    std::cout << Robust::Description::View(adr_result.model, description) << std::endl;

    model.use(Gurobi());
    model.optimize();
    std::cout << "Deterministic Problem has value: " << model.get_best_obj() << std::endl;

    model.use(
            Robust::AffineDecisionRule(description)
                    .with_deterministic_optimizer(Gurobi())
    );
    model.optimize();
    std::cout << "Affine Decision Rule Problem has value: " << model.get_best_obj() << std::endl;

    model.use(
            Robust::Deterministic(description)
                    .with_deterministic_optimizer(GLPK())
    );
    model.optimize();
    std::cout << "Robust Problem has value: " << model.get_best_obj() << std::endl;

    return 0;
}
