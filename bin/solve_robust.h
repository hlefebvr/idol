//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_SOLVE_ROBUST_H
#define IDOL_SOLVE_ROBUST_H

#include "Arguments.h"
#include "MethodManager.h"
#include "output.h"
#include "VariableAnalysis.h"
#include "idol/modeling.h"
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/robust/modeling/read_from_file.h"
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/BigMFreeSeparation.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/OptimalitySeparation.h"

class RobustMethodManager : public MethodManager {
public:
    RobustMethodManager() : MethodManager({
        { "CCG-FARKAS", { 50, "Column-and-constraint generation with Farkas-based separation; see Ayoub and Poss (2016) [https://doi.org/10.1007/s10287-016-0249-2]." } },
        { "CCG-MIBS", { 25, "Column-and-constraint generation with separation by MibS." } },
        { "CCG-KKT-SOS1", { 20, "Column-and-constraint generation with KKT-based separation using SOS1." } },
        { "YASOL", { 15, "Quantified programming formulation solved with Yasol; see Goerigk and Hartisch (2021) [https://doi.org/10.1016/j.cor.2021.105434]." } },
        //{ "BBBB-MIBS", { 5, "Bilevel-based branch-and-bound with MibS; see Lefebvre et al. (2023) [https://doi.org/10.1287/ijoc.2022.0086]." } },
        //{ "BBBB-KKT-SOS1", { 0, "Bilevel-based branch-and-bound with KKT using SOS1; see Lefebvre et al. (2023) [https://doi.org/10.1287/ijoc.2022.0086]." } }
    }) {}
};

struct UncertaintySetAnalysisResult : VariableAnalysisResult{
    bool is_zero_one_polytope = false;
    //bool has_rhs_uncerrtainty = false;
    //bool has_ctr_uncertainty = false;
};

inline UncertaintySetAnalysisResult get_uncertainty_set_analysis(const idol::Model& t_uncertainty_set) {

    UncertaintySetAnalysisResult result;

    for (const auto& var : t_uncertainty_set.vars()) {
        do_variable_analysis(result, t_uncertainty_set, var);
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
                                              const idol::Bilevel::Description& t_bilevel_description,
                                              const idol::Robust::Description& t_robust_description) {

    StageAnalysisResult result;

    for (const auto& var : t_model.vars()) {

        if (t_bilevel_description.is_upper(var)) {
            do_variable_analysis(result.first_stage, t_model, var);
        } else {
            do_variable_analysis(result.second_stage, t_model, var);
            result.has_second_stage = true;
        }

    }

    return result;
}

inline void solve_adjustable_robust(const Arguments& t_args) {

    using namespace idol;

    Env env;
    auto model = GLPK::read_from_file(env, t_args.file);
    auto bilevel_description = Bilevel::read_bilevel_description(model, t_args.aux_file);
    auto robust_description = Robust::read_from_file(model, t_args.uncertainty_param_file, t_args.uncertainty_set_file);
    const auto& uncertainty_set = robust_description.uncertainty_set();

    const auto uncertainty_analysis = get_uncertainty_set_analysis(uncertainty_set);
    const auto stage_analysis = get_stage_analysis(model, bilevel_description, robust_description);

    RobustMethodManager method_manager;

    if (stage_analysis.has_second_stage) {

        std::cout << "-- Detected: two-stage robust problem." << std::endl;

        if (stage_analysis.second_stage.all_bounded) {
            std::cout << "-- Detected: all second-stage variables are explicitly bounded." << std::endl;
        }

        if (!stage_analysis.second_stage.has_binary && !stage_analysis.second_stage.has_general_integer) {

            std::cout << "-- Detected: continuous second-stage decisions." << std::endl;

            method_manager.add("CCG-KKT-SOS1");

            if (!uncertainty_analysis.has_continuous) {

                method_manager.add("YASOL");
                method_manager.add("CCG-MIBS");

                if (!uncertainty_analysis.has_general_integer) {

                    std::cout << "-- Detected: binary uncertainty set." << std::endl;

                    if (stage_analysis.second_stage.all_bounded) {
                        method_manager.add("CCG-FARKAS");
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
                        method_manager.add("CCG-FARKAS");
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

                method_manager.add("CCG-MIBS");
                method_manager.add("YASOL");

            }

        }

    }

    method_manager.print_available_methods(t_args);

    const auto method = method_manager.get_method(t_args);

    std::cout << "-- Solving using " << method << std::endl;

    std::cout << "-- Solving problem using " << method << "." << std::endl;

    if (method.starts_with("CCG-")) {

        auto ccg = Robust::ColumnAndConstraintGeneration(robust_description, bilevel_description);
        ccg.with_initial_scenario_by_maximization(Gurobi());
        ccg.with_master_optimizer(Gurobi());
        ccg.with_logs(!t_args.mute);

        if (method == "CCG-FARKAS") {

            auto farkas = Robust::CCG::BigMFreeSeparation();
            farkas.with_single_level_optimizer(Gurobi());
            farkas.with_binary_uncertainty_set(!uncertainty_analysis.has_continuous || uncertainty_analysis.is_zero_one_polytope);

            ccg.add_separation(farkas);

        } else if (method == "CCG-KKT-SOS1") {

            auto kkt = Bilevel::KKT();
            kkt.with_sos1_constraints(true);
            kkt.with_single_level_optimizer(Gurobi());

            auto optimality_separation = Robust::CCG::OptimalitySeparation();
            optimality_separation.with_bilevel_optimizer(kkt);

            ccg.add_separation(optimality_separation);

        } else if (method == "CCG-MIBS") {

            auto mibs = Bilevel::MibS();

            auto optimality_separation = Robust::CCG::OptimalitySeparation();
            optimality_separation.with_bilevel_optimizer(mibs);

            ccg.add_separation(optimality_separation);

        }

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