//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_SOLVE_MILP_H
#define IDOL_SOLVE_MILP_H

#include "method-managers/MILPMethodManager.h"
#include "idol/bilevel/modeling/read_from_file.h"

#include "Arguments.h"
#include "output.h"
#include "idol/mixed-integer/optimizers/callbacks/ReducedCostFixing.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/RENS.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/JuMP/JuMP.h"

inline void solve_milp(const Arguments& t_args) {

    using namespace idol;

    MILPMethodManager method_manager;

    method_manager.print_available_methods(t_args);

    const auto method = method_manager.get_method(t_args);

    std::cout << "-- Solving using " << method << std::endl;

    Env env;
    auto model = GLPK::read_from_file(env, t_args.file);

    if (method == "GUROBI") {
        model.use(Gurobi());
    } else if (method == "GLPK") {
        model.use(GLPK());
    } else if (method == "HIGHS") {
        model.use(HiGHS());
    } else if (method == "JUMP") {
        model.use(JuMP(t_args.jump_optimizer));
    } else if (method == "CPLEX") {
        model.use(Cplex());
    } else if (method == "BNB") {
        auto bnb = BranchAndBound();
        bnb.with_branching_rule(MostInfeasible());
        bnb.with_node_selection_rule(BestBound());
        bnb.with_node_optimizer(*method_manager.get_sub_milp_optimizer(t_args, true));
        bnb.add_callback(ReducedCostFixing());
        bnb.add_callback(CglCutCallback());
        //bnb.add_callback(Heuristics::LocalMIP());
        bnb.add_callback(Heuristics::RENS().with_optimizer(
            BranchAndBound()
                .with_branching_rule(MostInfeasible())
                .with_node_selection_rule(BestBound())
                .add_callback(ReducedCostFixing())
                .add_callback(CglCutCallback())
                .with_node_optimizer(*method_manager.get_sub_milp_optimizer(t_args, true))
        ));
        bnb.with_logger(Logs::BranchAndBound::Info().with_frequency_in_seconds(0));
        model.use(bnb);
    }

    // Set Parameters
    model.optimizer().set_param_logs(!t_args.mute);
    model.optimizer().set_param_time_limit(t_args.time_limit);

    // Set Tolerances
    if (t_args.tol_feasibility >= 0.) { model.optimizer().set_tol_feasibility(t_args.tol_feasibility); }

    model.optimize();

    report_standard_output(model, t_args);

}

#endif //IDOL_SOLVE_MILP_H