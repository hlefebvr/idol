//
// Created by henri on 05.02.25.
//
#include <iostream>
#include <OsiCpxSolverInterface.hpp>
#include <OsiClpSolverInterface.hpp>
#include <OsiSymSolverInterface.hpp>
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
#include "idol/bilevel/modeling/Description.h"
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    //const auto instance = Problems::FLP::generate_instance_1991_Cornuejols_et_al(5, 10, 2);
    //std::ofstream file("ccg-discrete-uncertainty.data.txt");
    //file << instance;
    //file.close();

    // Read instance
    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("ccg-discrete-uncertainty.data.txt");
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    // Uncertainty set
    Model uncertainty_set(env);
    const double Gamma = 2;
    const auto xi = uncertainty_set.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Binary, 0., "xi");
    uncertainty_set.add_ctr(idol_Sum(i, Range(n_facilities), idol_Sum(j, Range(n_customers), xi[i][j])) <= Gamma);

    // Make model
    Model model(env);
    Robust::Description robust_description(uncertainty_set);
    Bilevel::Description bilevel_description(env);

    auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "x");
    auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Binary, 0., "y");

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        const auto c = model.add_ctr(
                idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i)
            );
        bilevel_description.make_lower_level(c);
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        const auto c = model.add_ctr(idol_Sum(i, Range(n_facilities), y[i][j]) >= 1);
        bilevel_description.make_lower_level(c);
    }

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            const auto c = model.add_ctr(y[i][j] <= x[i]);
            bilevel_description.make_lower_level(c);
        }
    }

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0; j < n_customers; ++j) {
            bilevel_description.make_lower_level(y[i][j]);
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
            robust_description.set_uncertain_rhs(c, -xi[i][j]); // models y_ij <= 1 - xi_j
            bilevel_description.make_lower_level(c);
        }
    }

    const auto mibs = Bilevel::MibS()
            .with_cplex_for_feasibility(true)
            .with_file_interface(false)
            .with_logs(false)
            ;

    model.use(
            Robust::ColumnAndConstraintGeneration(robust_description, bilevel_description)
                    //.with_initial_scenario_by_maximization(Gurobi())
                    //.with_initial_scenario_by_minimization(Gurobi())
                    .with_master_optimizer(Gurobi())
                    .add_feasibility_separation_optimizer(mibs)
                    .add_optimality_separation_optimizer(mibs)
                    .with_logs(true)
    );
    model.optimize();

    std::cout << "Status: " << model.get_status() << std::endl;
    std::cout << "Reason: " << model.get_reason() << std::endl;
    std::cout << "Objective value: " << model.get_best_obj() << std::endl;

    return 0;
}
