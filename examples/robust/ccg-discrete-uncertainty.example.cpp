//
// Created by henri on 05.02.25.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/problems/facility-location-problem/FLP_Instance.h"
#include "idol/mixed-integer/optimizers/callbacks/ReducedCostFixing.h"
#include "idol/robust/modeling/Description.h"
#include "idol/robust/optimizers/deterministic/Deterministic.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/mixed-integer/optimizers/padm/SubProblem.h"
#include "idol/mixed-integer/optimizers/padm/PenaltyUpdates.h"
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"
#include "idol/mixed-integer/optimizers/padm/PADM.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/robust/optimizers/benders/Benders.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    /*****************/
    /* Read Instance */
    /*****************/

    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("ccg-discrete-uncertainty.data.txt");
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    /****************************/
    /* Make Deterministic Model */
    /****************************/

    Model model(env);
    const auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "x");
    const auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Continuous, 0., "y");
    std::list<Ctr> second_stage_constraints;

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        const auto c = model.add_ctr(idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i));
        second_stage_constraints.emplace_back(c);
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        const auto c = model.add_ctr(idol_Sum(i, Range(n_facilities), y[i][j]) >= 1);
        second_stage_constraints.emplace_back(c);
    }

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            const auto c = model.add_ctr(y[i][j] <= x[i]);
            second_stage_constraints.emplace_back(c);
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

    /************************/
    /* Declare Second Stage */
    /************************/

    Bilevel::Description bilevel_description(env);
    for (const auto& y_ij : flatten<Var, 2>(y)) {
        bilevel_description.make_lower_level(y_ij);
    }
    for (const auto& ctr : second_stage_constraints) {
        bilevel_description.make_lower_level(ctr);
    }

    /**************************/
    /* Create Uncertainty Set */
    /**************************/

    Model uncertainty_set(env);
    const double Gamma = 2;
    const auto xi = uncertainty_set.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Binary, 0., "xi");
    uncertainty_set.add_ctr(idol_Sum(i, Range(n_facilities), idol_Sum(j, Range(n_customers), xi[i][j])) <= Gamma);

    /***********************/
    /* Declare Uncertainty */
    /***********************/

    Robust::Description robust_description(uncertainty_set);

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0; j < n_customers; ++j) {
            const auto c = model.add_ctr(y[i][j] <= 1);
            robust_description.set_uncertain_rhs(c, -xi[i][j]); // models y_ij <= 1 - xi_j
            bilevel_description.make_lower_level(c);
        }
    }

    /***********************************************************************************************/
    /* Creating a heuristic for the adversarial problem: PADM based on strong duality reformulation */
    /***********************************************************************************************/

    Annotation<double> initial_penalties(env, "initial_penalties", 1e1);
    Annotation<unsigned int> decomposition(env, "sub_problem", 0);
    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0; j < n_customers; ++j) {
            xi[i][j].set(decomposition, 1);
        }
    }

    const auto padm = Bilevel::StrongDuality()
                .with_single_level_optimizer(
                    PADM(decomposition, initial_penalties)
                        .with_default_sub_problem_spec(
                            ADM::SubProblem().with_optimizer(Gurobi())
                        )
                        .with_penalty_update(PenaltyUpdates::Multiplicative(2))
                        .with_rescaling_threshold(1e4)
                        .with_logs(false)
                )
            ;

    /**************************************************************/
    /* Creating an exact solver for the adversarial problem: MibS */
    /**************************************************************/

    const auto mibs = Bilevel::MibS()
            .with_cplex_for_feasibility(true)
            .with_logs(true)
            ;

    /************************************************/
    /* Creating the column-and-constraint optimizer */
    /************************************************/

    auto ccg = Robust::ColumnAndConstraintGeneration(robust_description, bilevel_description)

            .with_initial_scenario_by_maximization(Gurobi())
            .with_initial_scenario_by_minimization(Gurobi())

            .with_master_optimizer(Gurobi())

            .add_feasibility_separation_optimizer(padm)
            .add_feasibility_separation_optimizer(mibs)

            .add_optimality_separation_optimizer(padm)
            .add_optimality_separation_optimizer(mibs)
            .with_logs(true);

    auto benders = Robust::Benders(robust_description, bilevel_description)
            .add_feasibility_separation_optimizer(mibs)
            .add_optimality_separation_optimizer(mibs);

    model.use(
            benders
    );

    /***********/
    /* Solving */
    /***********/

    model.optimize();

    std::cout << "Status: " << model.get_status() << std::endl;
    std::cout << "Reason: " << model.get_reason() << std::endl;
    std::cout << "Objective value: " << model.get_best_obj() << std::endl;

    return 0;
}
