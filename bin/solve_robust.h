//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_SOLVE_ROBUST_H
#define IDOL_SOLVE_ROBUST_H

#include "Arguments.h"
#include "method-managers/MethodManager.h"
#include "output.h"
#include "VariableAnalysis.h"
#include "idol/modeling.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/robust/modeling/read_from_file.h"
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/BigMFreeSeparation.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/FeasibilitySeparation.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/OptimalitySeparation.h"
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/CriticalValueColumnAndConstraintGeneration.h"

class AdjustableRobustMethodManager : public MethodManager {
public:
    AdjustableRobustMethodManager() : MethodManager({
        /*** Adjustable robust problems ***/
        { "CCG-FARKAS", { 50, "Column-and-constraint generation with Farkas-based separation; see Ayoub and Poss (2016) [https://doi.org/10.1007/s10287-016-0249-2]." } },
        { "CCG-MIBS", { 25, "Column-and-constraint generation with separation by MibS." } },
        { "CCG-KKT-SOS1", { 20, "Column-and-constraint generation with KKT-based separation using SOS1." } },
        //{ "YASOL", { 15, "Quantified programming formulation solved with Yasol; see Goerigk and Hartisch (2021) [https://doi.org/10.1016/j.cor.2021.105434]." } },
        //{ "BBBB-MIBS", { 5, "Bilevel-based branch-and-bound with MibS; see Lefebvre et al. (2023) [https://doi.org/10.1287/ijoc.2022.0086]." } },
        //{ "BBBB-KKT-SOS1", { 0, "Bilevel-based branch-and-bound with KKT using SOS1; see Lefebvre et al. (2023) [https://doi.org/10.1287/ijoc.2022.0086]." } }

        /*** Static robust problems ***/
        { "CVCCG", { 140, "Critical-value column-and-constraint generation; see Lozano and Borrero (2025) [https://link.springer.com/article/10.1007/s10107-025-02249-6]." } },
        { "GEN-IND", { 130, "Scenario generation (indicator functions in case of decision-dependent uncertainty sets)." } }
    }) {}
};

struct UncertaintySetAnalysisResult : VariableAnalysisResult{
    bool is_zero_one_polytope = false;
    bool has_integer_linking_variables = true;
    bool has_binary_linking_variables = true;
    bool has_integer_coefficients = true;
    bool has_decision_dependence = false;
    //bool has_rhs_uncerrtainty = false;
    //bool has_ctr_uncertainty = false;
};

inline UncertaintySetAnalysisResult get_uncertainty_set_analysis(const idol::Model& t_model, const idol::Model& t_uncertainty_set) {

    UncertaintySetAnalysisResult result;

    for (const auto& var : t_uncertainty_set.vars()) {
        do_variable_analysis(result, t_uncertainty_set, var);
    }

    for (const auto& var : t_uncertainty_set.vars()) {

        if (!t_model.has(var)) {
            continue;
        }

        result.has_decision_dependence = true;
        const auto type = t_uncertainty_set.get_var_type(var);
        if (type != idol::Binary) {
            result.has_binary_linking_variables = false;
        }
        if (type == idol::Continuous) {
            result.has_integer_linking_variables = false;
        }
    }

    return result;
}

struct StageAnalysisResult {
    struct FirstStage : VariableAnalysisResult {};
    struct SecondStage : VariableAnalysisResult {};

    FirstStage first_stage;
    SecondStage second_stage;
    bool has_second_stage = false;

};

inline StageAnalysisResult get_stage_analysis(const idol::Model& t_model,
                                              const std::optional<idol::Bilevel::Description>& t_bilevel_description,
                                              const idol::Robust::Description& t_robust_description) {

    StageAnalysisResult result;

    for (const auto& var : t_model.vars()) {

        if (!t_bilevel_description || t_bilevel_description->is_upper(var)) {
            do_variable_analysis(result.first_stage, t_model, var);
        } else {
            do_variable_analysis(result.second_stage, t_model, var);
            result.has_second_stage = true;
        }

    }

    return result;
}

inline void solve_robust(const Arguments& t_args) {

    using namespace idol;

    Env env;
    auto model = Model::read_from_file(env, t_args.file);
    auto robust_description = Robust::read_from_file(model, t_args.uncertainty_param_file, t_args.uncertainty_set_file);
    const auto& uncertainty_set = robust_description.uncertainty_set();

    std::optional<Bilevel::Description> bilevel_description;
    if (!t_args.aux_file.empty()) {
        bilevel_description = Bilevel::read_bilevel_description(model, t_args.aux_file);
    }

    const auto uncertainty_analysis = get_uncertainty_set_analysis(model, uncertainty_set);
    const auto stage_analysis = get_stage_analysis(model, bilevel_description, robust_description);

    AdjustableRobustMethodManager robust_method_manager;
    MILPMethodManager sub_milp_method_manager;

    if (stage_analysis.has_second_stage) {

        std::cout << "-- Detected: two-stage robust problem." << std::endl;

        if (stage_analysis.second_stage.all_bounded) {
            std::cout << "-- Detected: all second-stage variables are explicitly bounded." << std::endl;
        }

        if (!stage_analysis.second_stage.has_binary && !stage_analysis.second_stage.has_general_integer) {

            std::cout << "-- Detected: continuous second-stage decisions." << std::endl;

            robust_method_manager.add("CCG-KKT-SOS1");

            if (!uncertainty_analysis.has_continuous) {

                //robust_method_manager.add("YASOL");
                robust_method_manager.add("CCG-MIBS");

                if (!uncertainty_analysis.has_general_integer) {

                    std::cout << "-- Detected: binary uncertainty set." << std::endl;

                    if (stage_analysis.second_stage.all_bounded) {
                        robust_method_manager.add("CCG-FARKAS");
                        //method_manager.add("BBBB-MIBS");
                    }

                    //method_manager.add("BBBB-KKT-SOS1");

                } else {

                    std::cout << "-- Detected: continuous uncertainty set." << std::endl;

                }

            } else  if (!uncertainty_analysis.has_binary && !uncertainty_analysis.has_general_integer) {

                std::cout << "-- Detected: continuous uncertainty set." << std::endl;

                if (uncertainty_analysis.is_zero_one_polytope) {

                    std::cout << "-- Detected: zero-one polytope uncertainty set" << std::endl;

                    if (stage_analysis.second_stage.all_bounded) {
                        robust_method_manager.add("CCG-FARKAS");
                    }

                    //method_manager.add("BBBB-KKT-SOS1");

                }

            } else {

                std::cout << "-- Detected: mixed-integer uncertainty set." << std::endl;

            }

        } else {

            std::cout << "-- Detected: (mixed-)integer second-stage decisions." << std::endl;

            if (uncertainty_analysis.has_continuous) {

                std::cout << "-- Detected: (mixed-)integer uncertainty set." << std::endl;

                std::cerr << "You are trying to solve a two-stage robust problem with mixed-integer second-stage decisions and uncertainty set that contains continuous variables.\n"
                    "In general, this is a min-sup-min and no method is implemented." << std::endl;

            } else {

                if (!uncertainty_analysis.has_general_integer) {
                    std::cout << "-- Detected: binary uncertainty set." << std::endl;
                    //method_manager.add("BBBB-MIBS");
                } else {
                    std::cout << "-- Detected: integer uncertainty set." << std::endl;
                }

                robust_method_manager.add("CCG-MIBS");
                //robust_method_manager.add("YASOL");

            }

        }

    } else { // Static robust problem

        if (uncertainty_analysis.has_decision_dependence) {
            std::cout << "-- Detected: uncertainty set is decision-dependent." << std::endl;
            if (uncertainty_analysis.has_integer_coefficients) {
                std::cout << "-- Detected: uncertainty set has integer coefficients." << std::endl;
                if (uncertainty_analysis.has_integer_linking_variables) {
                    std::cout << "-- Detected: uncertainty set has integer linking variables." << std::endl;
                    robust_method_manager.add("CVCCG");
                }
            }
        }

        if (!uncertainty_analysis.has_decision_dependence || uncertainty_analysis.has_binary_linking_variables) {
            std::cout << "-- Detected: uncertainty set has binary linking variables." << std::endl;
            robust_method_manager.add("GEN-IND");
        }

    }

    robust_method_manager.print_available_methods(t_args);

    const auto method = robust_method_manager.get_method(t_args);
    const auto sub_milp_optimizer = sub_milp_method_manager.get_sub_milp_optimizer(t_args);

    std::cout << "-- Solving problem using " << method << "." << std::endl;
    std::cout << "-- Sub-MILP method is " << sub_milp_method_manager.get_sub_milp_method(t_args) << "." << std::endl;

    if (method.starts_with("CCG-")) {

        auto ccg = Robust::ColumnAndConstraintGeneration(robust_description, *bilevel_description);
        ccg.with_initial_scenario_by_maximization(*sub_milp_optimizer);
        ccg.with_master_optimizer(*sub_milp_optimizer);
        ccg.with_logs(!t_args.mute);

        if (method == "CCG-FARKAS") {

            auto farkas = Robust::CCG::BigMFreeSeparation();
            farkas.with_single_level_optimizer(*sub_milp_optimizer);
            farkas.with_binary_uncertainty_set(!uncertainty_analysis.has_continuous || uncertainty_analysis.is_zero_one_polytope);

            ccg.add_separation(farkas);

        } else if (method == "CCG-KKT-SOS1") {

            auto kkt = Bilevel::KKT();
            kkt.with_sos1_constraints(true);
            kkt.with_single_level_optimizer(*sub_milp_optimizer);

            if (!t_args.complete_recourse) {
                auto feasibility_separation = Robust::CCG::FeasibilitySeparation();
                feasibility_separation.with_bilevel_optimizer(kkt);

                ccg.add_separation(feasibility_separation);
            }

            auto optimality_separation = Robust::CCG::OptimalitySeparation();
            optimality_separation.with_bilevel_optimizer(kkt);

            ccg.add_separation(optimality_separation);

        } else if (method == "CCG-MIBS") {

            auto mibs = Bilevel::MibS();
            mibs.with_feasibility_checker(*sub_milp_optimizer);

            if (!t_args.complete_recourse) {
                std::cout << "-- The problem is not known to have complete recourse, adding feasibility separation." << std::endl;
                auto feasibility_separation = Robust::CCG::FeasibilitySeparation();
                feasibility_separation.with_bilevel_optimizer(mibs);

                ccg.add_separation(feasibility_separation);
            }

            auto optimality_separation = Robust::CCG::OptimalitySeparation();
            optimality_separation.with_bilevel_optimizer(mibs);

            ccg.add_separation(optimality_separation);

        }

        model.use(ccg);

    } else if (method == "CVCCG") {

        const auto milp_solver = sub_milp_method_manager.get_sub_milp_optimizer(t_args);

        auto ccg = Robust::CriticalValueColumnAndConstraintGeneration(robust_description);
        ccg.with_master_optimizer(*milp_solver);
        ccg.with_deterministic_optimizer(*milp_solver);

        model.use(ccg);

    } else if (method == "GEN-IND") {
        auto ccg = Robust::CriticalValueColumnAndConstraintGeneration(robust_description);
        ccg.with_master_optimizer(*sub_milp_optimizer);
        ccg.with_deterministic_optimizer(*sub_milp_optimizer);
        ccg.with_indicator(true);

        model.use(ccg);
    }

    // Set Parameters
    model.optimizer().set_param_logs(!t_args.mute);
    model.optimizer().set_param_time_limit(t_args.time_limit);

    // Set Tolerances
    if (t_args.tol_feasibility >= 0.) { model.optimizer().set_tol_feasibility(t_args.tol_feasibility); }

    model.optimize();

    report_standard_output(model, t_args);

}

#endif //IDOL_SOLVE_ROBUST_H