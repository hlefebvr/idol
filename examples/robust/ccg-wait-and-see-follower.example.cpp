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
#include "idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    /*****************/
    /* Read Instance */
    /*****************/

    if (false) {
        const auto ginstance = Problems::FLP::generate_instance_1991_Cornuejols_et_al(5, 5, 2);

        std::ofstream file("ccg-wait-and-see-follower.data.txt");
        assert(file.is_open());
        file << ginstance;
        file.close();
    }

    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("ccg-wait-and-see-follower.data.txt");
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
    bilevel_description.set_lower_level_obj(model.get_obj_expr());

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

    /**************************************************************/
    /* Creating an exact solver for the adversarial problem: MibS */
    /**************************************************************/

    const Annotation<double> big_M(env, "big_M", 1e3);
    const auto kkt = Bilevel::KKT()
                .with_single_level_optimizer(Gurobi())
                //.with_big_M(big_M)
                .with_logs(false)
            ;

    const auto mibs = Bilevel::MibS()
            .with_cplex_for_feasibility(true)
            .with_logs(false)
            ;

    /************************************************/
    /* Creating the column-and-constraint optimizer */
    /************************************************/

    model.use(
            Robust::ColumnAndConstraintGeneration(robust_description, bilevel_description)

                    .with_initial_scenario_by_maximization(Gurobi())
                    .with_initial_scenario_by_minimization(Gurobi())

                    .with_master_optimizer(kkt)

                    .add_feasibility_separation_optimizer(kkt)

                    .add_optimality_separation_optimizer(Bilevel::PessimisticAsOptimistic() + kkt)
                    .with_logs(true)
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
