//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_SOLVE_MILP_H
#define IDOL_SOLVE_MILP_H

#include "method-managers/milp/MILPMethodManager.h"
#include "idol/bilevel/modeling/read_from_file.h"

#include "Arguments.h"
#include "output.h"

inline void milp(const Arguments& t_args) {

    using namespace idol;

    Env env;

    // Read model
    std::optional<Model> model;
    if (!t_args.file.empty()) {
        std::cout << "-- Read main model from " << t_args.file << std::endl;
        model.emplace(Model::read_from_file(env, t_args.file));
    }

    /*
     if (method == "BNB") {
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
        model->use(bnb);
    }
    */

    MILPMethodManager manager(t_args);

    if (model) {
        manager.set_problem(*model);
    }

    if (t_args.sub_command == List) {
        manager.print_details();
        exit(0);
    }

    if (!model) {
        std::cerr << "No model was given (please, provide at least: .lp/.mps)." << std::endl;
        exit(1);
    }

    if (t_args.sub_command == Solve) {

        manager.set_optimizer(*model);

        // Set Parameters
        model->optimizer().set_param_logs(!t_args.mute);
        model->optimizer().set_param_time_limit(t_args.time_limit);

        // Set Tolerances
        if (t_args.tol_feasibility >= 0.) { model->optimizer().set_tol_feasibility(t_args.tol_feasibility); }

        model->optimize();

        report_standard_output(*model, t_args);

        exit(0);
    }

    std::cerr << "Nothing to do with sub-command " << t_args.sub_command << "." << std::endl;
    exit(1);

}

#endif //IDOL_SOLVE_MILP_H