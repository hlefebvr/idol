//
// Created by henri on 28.11.24.
//
#include <iostream>
#include "idol/modeling.h"
#include "idol/bilevel/optimizers/BranchAndCut/BranchAndCut.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/mixed-integer/problems/facility-location-problem/FLP_Instance.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/robust/modeling/Description.h"
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/BigMFreeSeparation.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/FeasibilitySeparation.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/OptimalitySeparation.h"
#include "idol/robust/optimizers/nested-branch-and-cut/BilevelBasedBranchAndBound.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    // Read instance
    //const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("flp-nbc.data.txt");
    const auto instance = Problems::FLP::generate_instance_1991_Cornuejols_et_al(2, 1, 2);
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    std::ofstream file("flp-nbc.data.txt");
    assert(file.is_open());
    file << instance;
    file.close();

    // Uncertainty set
    Model uncertainty_set(env);
    const double Gamma = 1;
    const auto u = uncertainty_set.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "u");
    uncertainty_set.add_ctr(idol_Sum(i, Range(n_facilities), u[i]) <= Gamma);

    // Make model
    Model model(env);
    Robust::Description robust_description(uncertainty_set);
    Bilevel::Description bilevel_description(env);

    const auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "x");
    const auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Binary, 0., "y");

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add_ctr(idol_Sum(j, Range(n_customers), std::ceil(instance.demand(j)) * y[i][j]) <= std::ceil(instance.capacity(i)));
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
                                std::ceil(instance.fixed_cost(i)) * x[i]
                                + idol_Sum(j, Range(n_customers),
                                           std::ceil(instance.per_unit_transportation_cost(i, j) * instance.demand(j)) *
                                           y[i][j]
                                )
                       )
    );

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0; j < n_customers; ++j) {
            const auto c = model.add_ctr(y[i][j] <= 1);
            robust_description.set_uncertain_rhs(c, -u[i]); // models y_ij <= 1 - u_i
            bilevel_description.make_lower_level(y[i][j]);
        }
    }

    for (const auto& ctr : model.ctrs()) {
        bilevel_description.make_lower_level(ctr);
    }

    auto bilevel_bnc = Bilevel::BranchAndCut();
    bilevel_bnc.with_sub_problem_optimizer(Gurobi());
    bilevel_bnc.with_logs(true);

    auto mibs = Bilevel::MibS();
    mibs.with_cplex_for_feasibility(true);

    auto nested_branch_and_cut = Robust::BilevelBasedBranchAndBound(robust_description, bilevel_description);
    nested_branch_and_cut.with_optimality_bilevel_optimizer(mibs);
    nested_branch_and_cut.with_logs(true);

    model.use(nested_branch_and_cut);
    model.optimize();

    std::cout << "Status: " << model.get_status() << std::endl;
    std::cout << "Objective: " << model.get_best_obj() << std::endl;

    std::cout << "----------------------------------" << std::endl;

    auto feasibility_separation = Robust::CCG::FeasibilitySeparation();
    feasibility_separation.with_bilevel_optimizer(mibs);
    feasibility_separation.with_integer_slack_variables(true);

    auto optimality_separation = Robust::CCG::OptimalitySeparation();
    optimality_separation.with_bilevel_optimizer(mibs);

    auto ccg = Robust::ColumnAndConstraintGeneration(robust_description, bilevel_description);
    ccg.with_master_optimizer(Gurobi());
    ccg.add_separation(feasibility_separation);
    ccg.add_separation(optimality_separation);
    ccg.with_logs(true);

    model.use(ccg);
    model.optimize();

    std::cout << "Status: " << model.get_status() << std::endl;
    std::cout << "Objective: " << model.get_best_obj() << std::endl;


    return 0;
}
