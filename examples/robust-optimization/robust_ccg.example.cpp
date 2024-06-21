//
// Created by henri on 20.06.24.
//
#include <iostream>
#include <Research/idol/lib/include/idol/modeling.h>
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"
#include "idol/modeling/bilevel-optimization/LowerLevelDescription.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h"
#include "idol/modeling/bilevel-optimization/read_from_file.h"
#include "idol/problems/facility-location-problem/FLP_Instance.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/separators/Bilevel.h"
#include "idol/modeling/robust-optimization/StageDescription.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/stabilizers/TrustRegion.h"

using namespace idol;

std::pair<Model, Vector<Var, 1>>
create_uncertainty_set(Env& t_env,
                       const Problems::FLP::Instance& t_instance,
                       double t_Gamma);

Model create_deterministic_model(Env& t_env,
                                 const Problems::FLP::Instance& t_instance,
                                 const Vector<Var, 1>& t_xi,
                                 double t_percentage_increase);

Robust::StageDescription create_stage_description(const Model& t_model);

int main(int t_argc, const char** t_argv) {

    Env env;

    // Read instance
    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("robust_ccg.data.txt");
    const double Gamma = 3;
    const double percentage_increase = .2;

    // Create models
    auto [uncertainty_set, xi] = create_uncertainty_set(env, instance, Gamma);
    auto model = create_deterministic_model(env, instance, xi, percentage_increase);
    auto stages = create_stage_description(model);

    model.use(
            Robust::ColumnAndConstraintGeneration(stages, uncertainty_set)
                    .with_master_optimizer(Gurobi())
                    .with_separator(
                            Robust::CCGSeparators::Bilevel()
                    )
                    //.with_stabilization(Robust::CCGStabilizers::TrustRegion())
                    .with_logs(true)
    );

    model.optimize();

    std::cout << save_primal(model) << std::endl;

    return 0;
}


std::pair<Model, Vector<Var, 1>>
create_uncertainty_set(Env& t_env, const Problems::FLP::Instance& t_instance, double t_Gamma) {

    const unsigned int n_customers = t_instance.n_customers();

    Model uncertainty_set(t_env);

    auto xi = uncertainty_set.add_vars(Dim<1>(n_customers), 0., 1, Binary, "xi");
    uncertainty_set.add_ctr(idol_Sum(j, Range(n_customers), xi[j]) <= t_Gamma);

    return {
            std::move(uncertainty_set),
            std::move(xi)
    };
}

Model
create_deterministic_model(Env& t_env, const Problems::FLP::Instance& t_instance, const Vector<Var, 1>& t_xi, double t_percentage_increase) {

    const unsigned int n_customers = t_instance.n_customers();
    const unsigned int n_facilities = t_instance.n_facilities();

    Model result(t_env);

    const auto x = result.add_vars(Dim<1>(n_facilities), 0., 1., Binary, "x");
    const auto y = result.add_vars(Dim<2>(n_facilities, n_customers), 0., Inf, Continuous, "y");
    const auto s = result.add_vars(Dim<1>(n_customers), 0., Inf, Continuous, "s");

    // Capacity constraints
    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        result.add_ctr(idol_Sum(j, Range(n_customers), y[i][j]) <= t_instance.capacity(i) * x[i]);
    }

    // Demand satisfaction constraints
    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        result.add_ctr(idol_Sum(i, Range(n_facilities), y[i][j]) + s[j] == t_instance.demand(j) * (1 + t_percentage_increase * !t_xi[j]));
    }

    // Objective function
    result.set_obj_expr(idol_Sum(i, Range(n_facilities),
                                t_instance.fixed_cost(i) * x[i]
                                + idol_Sum(j, Range(n_customers),
                                           t_instance.per_unit_transportation_cost(i, j) * y[i][j]
                                )
                       )
                        + idol_Sum(j, Range(n_customers), 1e4 * s[j])
    );

    return std::move(result);
}

Robust::StageDescription create_stage_description(const Model& t_model) {

    Robust::StageDescription result(t_model.env());

    for (const auto& var : t_model.vars()) {
        if (var.name().front() != 'x') {
            result.set_stage(var, 2);
        }
    }

    for (const auto& ctr : t_model.ctrs()) {
        result.set_stage(ctr, 2);
    }

    return result;
}
