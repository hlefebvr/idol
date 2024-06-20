//
// Created by henri on 20.06.24.
//
#include <iostream>
#include <Research/idol/lib/include/idol/modeling.h>
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"
#include "idol/modeling/bilevel-optimization/Description.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h"
#include "idol/modeling/bilevel-optimization/read_from_file.h"
#include "idol/problems/facility-location-problem/FLP_Instance.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/separators/Bilevel.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    Env env;

    // Read instance
    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("robust_ccg.data.txt");
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();
    const double Gamma = 0;
    const double percentage_increase = .2;

    // Make Uncertainty Set
    Model uncertainty_set(env);
    auto xi = uncertainty_set.add_vars(Dim<1>(n_customers), 0., 1, Binary, "xi");
    uncertainty_set.add_ctr(idol_Sum(j, Range(n_customers), xi[j]) <= Gamma);

    Annotation<Var> second_stage_variables(env, "second_stage_variables", MasterId);
    Annotation<Ctr> second_stage_constraints(env, "second_stage_constraints", MasterId);

    // Make model
    Model model(env);

    auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, "x");
    auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., Inf, Continuous, "y");
    auto s = model.add_vars(Dim<1>(n_customers), 0., Inf, Continuous, "s");

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        auto c = model.add_ctr(
                idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i) * x[i],
                "capacity_" + std::to_string(i));
        c.set(second_stage_constraints, 0);
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        auto c = model.add_ctr(
                idol_Sum(i, Range(n_facilities), y[i][j]) + s[j] == instance.demand(j) * (1 + percentage_increase * !xi[j]),
                  "demand_" + std::to_string(j));
        c.set(second_stage_constraints);
    }

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0; j < n_customers; ++j) {
            y[i][j].set(second_stage_variables, 0);
        }
    }

    for (unsigned int j = 0; j < n_customers; ++j) {
        s[j].set(second_stage_variables, 0);
    }

    model.set_obj_expr(idol_Sum(i, Range(n_facilities),
                                instance.fixed_cost(i) * x[i]
                                + idol_Sum(j, Range(n_customers),
                                           instance.per_unit_transportation_cost(i, j) * y[i][j]
                                )
                       )
                       + idol_Sum(j, Range(n_customers), 1e4 * s[j])
    );

    model.use(
            Robust::ColumnAndConstraintGeneration(
                        second_stage_variables,
                        second_stage_constraints,
                        uncertainty_set
                    )
                    .with_master_optimizer(Gurobi())
                    .with_separator(Robust::CCGSeparators::Bilevel())
                    .with_logs(true)
    );

    model.optimize();

    if (model.get_status() != Optimal) {
        std::cout << "No Optimal Solution Found" << std::endl;
        return 0;
    }
    std::cout << save_primal(model) << std::endl;

    return 0;
}
