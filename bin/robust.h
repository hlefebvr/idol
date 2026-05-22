//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_SOLVE_ROBUST_H
#define IDOL_SOLVE_ROBUST_H

#include "Arguments.h"
#include "output.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/robust/modeling/read_from_file.h"
#include "method-managers/robust/RobustMethodManager.h"

inline void robust(const Arguments& t_args) {

    using namespace idol;

    Env env;

    // Read model
    std::optional<Model> model;
    if (!t_args.file.empty()) {
        std::cout << "-- Read main model from " << t_args.file << std::endl;
        model.emplace(Model::read_from_file(env, t_args.file));
    }

    // Read robust description
    std::optional<Robust::Description> robust_description;
    if (!t_args.uncertainty_param_file.empty() && !t_args.uncertainty_set_file.empty()) {
        std::cout << "-- Read uncertainty parameterization from " << t_args.uncertainty_param_file << std::endl;
        std::cout << "-- Read uncertainty set from " << t_args.uncertainty_set_file << std::endl;
        robust_description.emplace(Robust::read_from_file(*model, t_args.uncertainty_param_file, t_args.uncertainty_set_file));
    }

    // Read bilevel description
    std::optional<Bilevel::Description> bilevel_description;
    if (!t_args.aux_file.empty()) {
        std::cout << "-- Read stage description from " << t_args.aux_file << std::endl;
        bilevel_description = Bilevel::read_bilevel_description(*model, t_args.aux_file);
    }

    RobustMethodManager robust_method_manager(t_args);

    if (model && robust_description) {
        robust_method_manager.set_problem(*model, *robust_description, bilevel_description);
    }

    std::cout << robust_description->uncertainty_set() << std::endl;

    if (t_args.sub_command == List) {
        robust_method_manager.print_details();
        exit(0);
    }

    if (!model || !robust_description) {
        std::cerr << "No robust model was given (please, provide at least: .lp/.mps, --aux and --unc)." << std::endl;
        exit(1);
    }

    if (t_args.sub_command == Solve) {

        robust_method_manager.set_optimizer(*model);

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

    /*
    if (method.name() == "ROCPP-KADAPT") {
        auto rocpp = Robust::ROCPP::KAdaptability(robust_description, *bilevel_description);

        model.use(rocpp);
    }
    */

}

#endif //IDOL_SOLVE_ROBUST_H