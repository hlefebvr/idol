//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_SOLVE_BILEVEL_H
#define IDOL_SOLVE_BILEVEL_H

#include "Arguments.h"
#include "method-managers/MethodManager.h"
#include "output.h"
#include "VariableAnalysis.h"
#include "idol/modeling.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/bilevel/optimizers/PADM/PADM.h"
#include "idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h"
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/bilevel/problems/counterfactual/Counterfactual_Instance.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

class BilevelMethodManager : public MethodManager {
public:
    BilevelMethodManager() : MethodManager({
        { "MIBS", { 100, "MibS [https://github.com/coin-or/MibS]" } },
        { "KKT-NLP", { 130, "Solves via the KKT-based single-level reformulation as an NLP" } },
        { "KKT-SOS1", { 140, "Solves via the KKT-based single-level reformulation using SOS1 constraints" } },
        { "KKT-BIGM", { 150, "Solves via the KKT-based single-level reformulation using user-provided Big-M values" } },
        { "STRONG_DUALITY", { 90, "Solves via the strong-duality-based single-level reformulation as an NLP" } },
        { "PADM", { 40, "Computes a feasible point using a penalty alternating direction method." } },
    }) {}
};

struct BilevelAnalysisResult {
    struct Leader : VariableAnalysisResult {};
    struct Follower : VariableAnalysisResult {};

    Leader leader;
    Follower follower;
    bool has_coupling_constraints = false;
    bool has_continuous_linking_variables = false;
    bool has_binary_linking_variables = false;
    bool has_general_integer_linking_variables = false;
};

inline BilevelAnalysisResult get_bilevel_analysis(const idol::Model& t_model, const idol::Bilevel::Description& t_bilevel_description) {

    BilevelAnalysisResult result;

    // Do leader/follower analysis
    for (const auto& var : t_model.vars()) {

        if (t_bilevel_description.is_upper(var)) {
            do_variable_analysis(result.leader, t_model, var);
        } else {
            do_variable_analysis(result.follower, t_model, var);
        }

    }

    // Detect coupling and linking constraints
    for (const auto& ctr : t_model.ctrs()) {
        if (t_bilevel_description.is_upper(ctr)) {

            for (const auto& [var, coeff] : t_model.get_ctr_row(ctr)) {
                if (t_bilevel_description.is_lower(var)) {
                    result.has_coupling_constraints = true;
                    break;
                }
            }

        } else {

            for (const auto& [var, coeff] : t_model.get_ctr_row(ctr)) {
                if (t_bilevel_description.is_lower(var)) {
                    continue;
                }
                const auto type = t_model.get_var_type(var);
                const double lb = t_model.get_var_lb(var);
                const double ub = t_model.get_var_ub(var);
                if (type == idol::Continuous) {
                    result.has_continuous_linking_variables = true;
                } else if (type == idol::Binary) {
                    result.has_binary_linking_variables = true;
                } else if (type == idol::Integer) {
                    if (lb >= -.5 && ub <= 1.5) {
                        result.has_binary_linking_variables = true;
                    } else {
                        result.has_general_integer_linking_variables = true;
                    }
                }
            }

        }
    }

    return result;
}

inline void solve_bilevel(const Arguments& t_args) {

    using namespace idol;

    Env env;

    auto model = Model::read_from_file(env, t_args.file);
    const bool is_counterfactual = !t_args.ce_file.empty();

    std::optional<Bilevel::Description> bilevel_description;
    if (is_counterfactual) {
        const auto instance = Problems::Counterfactual::Instance(t_args.ce_file, t_args.file);
        bilevel_description = Bilevel::make_weak_CE_bilevel_model(model, instance);
    } else {
        bilevel_description = Bilevel::read_bilevel_description(model, t_args.aux_file);
    }

    const auto analysis = get_bilevel_analysis(model, *bilevel_description);

    BilevelMethodManager method_manager;
    MILPMethodManager submilp_method_manager;
    auto submilp_optimizer = submilp_method_manager.get_sub_milp_optimizer(t_args, false);

    if (analysis.has_general_integer_linking_variables || analysis.has_binary_linking_variables) {
        std::cout << "-- Detected: binary or integer linking variables only" << std::endl;
        method_manager.add("MIBS");
    }

    if (!analysis.follower.has_binary && !analysis.follower.has_general_integer) {
        std::cout << "-- Detected: continuous lower-level variables" << std::endl;
        method_manager.add("KKT-NLP");
        method_manager.add("KKT-SOS1");
        method_manager.add("STRONG_DUALITY");
        method_manager.add("PADM");
        if (!t_args.bound_provider.empty()) {
            method_manager.add("KKT-BIGM");
        }
    }

    method_manager.print_available_methods(t_args);

    const auto method = method_manager.get_method(t_args);

    std::cout << "-- Solving using " << method << std::endl;

    std::unique_ptr<OptimizerFactory> optimizer_factory;
    if (method == "MIBS") {
        auto mibs = Bilevel::MibS(*bilevel_description);
        optimizer_factory.reset(mibs.clone());
    } else if (method == "STRONG_DUALITY") {
        auto strong_duality = Bilevel::StrongDuality(*bilevel_description);
        strong_duality.with_single_level_optimizer(*submilp_optimizer);
        optimizer_factory.reset(strong_duality.clone());
    } else if (method.starts_with("KKT-")) {

        auto kkt = Bilevel::KKT(*bilevel_description);

        if (method == "KKT-NLP") {
            // Nothing to do here
        } else if (method == "KKT-BIGM") {
            auto bound_provider = Reformulators::KKT::BoundProviderMap::from_file(t_args.bound_provider);
            kkt.with_bound_provider(bound_provider);
        } else if (method == "KKT-SOS1") {
            kkt.with_sos1_constraints(true);
        }

        kkt.with_single_level_optimizer(*submilp_optimizer);

        if (!t_args.no_kleinert_vi && analysis.leader.all_bounded) {
            std::cout << "-- Using Kleinert et al. (2020) valid inequality [https://doi.org/10.1007/s11590-020-01660-6]." << std::endl;
            kkt.with_kleinart_cuts(analysis.leader.all_bounded);
        }

        optimizer_factory.reset(kkt.clone());

    } else if (method == "PADM") {

        auto padm = Bilevel::PADM(*bilevel_description);
        padm.with_single_level_optimizer(*submilp_optimizer);

        optimizer_factory.reset(padm.clone());

    }

    if (t_args.pessimistic) {
        std::cout << "-- Solving the pessimistic version of the problem" << std::endl;
        model.use(Bilevel::PessimisticAsOptimistic(*bilevel_description) + *optimizer_factory);
    } else {
        std::cout << "-- Solving the optimistic version of the problem" << std::endl;
        model.use(*optimizer_factory);
    }

    // Set Parameters
    model.optimizer().set_param_logs(!t_args.mute);
    model.optimizer().set_param_time_limit(t_args.time_limit);

    // Set Tolerances
    if (t_args.tol_feasibility >= 0.) { model.optimizer().set_tol_feasibility(t_args.tol_feasibility); }

    model.optimize();

    report_standard_output(model, t_args);

}

#endif //IDOL_SOLVE_BILEVEL_H