//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_SOLVE_BILEVEL_H
#define IDOL_SOLVE_BILEVEL_H

#include "Arguments.h"
#include "output.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h"
#include "idol/bilevel/problems/counterfactual/Counterfactual_Instance.h"
#include "method-managers/bilevel/BilevelMethodManager.h"

inline void bilevel(const Arguments& t_args) {

    using namespace idol;

    Env env;

    // Read model
    std::optional<Model> model;
    if (!t_args.file.empty()) {
        std::cout << "-- Read main model from " << t_args.file << std::endl;
        model.emplace(Model::read_from_file(env, t_args.file));
    }

    // Read bilevel description
    std::optional<Bilevel::Description> bilevel_description;
    if (!t_args.ce_file.empty()) {
        std::cout << "-- Read CE instance from " << t_args.aux_file << std::endl;
        const auto instance = Problems::Counterfactual::Instance(t_args.ce_file, t_args.file);
        bilevel_description = Bilevel::make_weak_CE_bilevel_model(*model, instance);
    } else if (!t_args.aux_file.empty()) {
        std::cout << "-- Read bilevel description (aux file) from " << t_args.aux_file << std::endl;
        bilevel_description = Bilevel::read_bilevel_description(*model, t_args.aux_file);
    }

    BilevelMethodManager manager(t_args);

    if (model && bilevel_description) {
        manager.set_problem(*model, *bilevel_description);
    }

    if (t_args.sub_command == List) {
        manager.print_details();
        exit(0);
    }

    if (!model || !bilevel_description) {
        std::cerr << "No robust model was given (please, provide at least: .lp/.mps, --aux and --unc)." << std::endl;
        exit(1);
    }

    if (t_args.sub_command == Solve) {

        const auto optimizer = manager.get_optimizer();

        if (t_args.pessimistic) {
            std::cout << "-- Considering the pessimistic version of the problem." << std::endl;
            model->use(Bilevel::PessimisticAsOptimistic(*bilevel_description) + *optimizer);
        } else {
            std::cout << "-- Considering the optimistic version of the problem." << std::endl;
            model->use(*optimizer);
        }

        // Set Parameters
        model->optimizer().set_param_logs(!t_args.mute);
        model->optimizer().set_param_time_limit(t_args.time_limit);

        // Set Tolerances
        if (t_args.tol_feasibility >= 0.) { model->optimizer().set_tol_feasibility(t_args.tol_feasibility); }

        model->optimize();

        report_standard_output(*model, t_args);
    }

    std::cerr << "Nothing to do with sub-command " << t_args.sub_command << "." << std::endl;
    exit(1);

}

#endif //IDOL_SOLVE_BILEVEL_H