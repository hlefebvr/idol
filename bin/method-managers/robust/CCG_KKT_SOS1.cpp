//
// Created by Henri on 19/05/2026.
//

#include "CCG_KKT_SOS1.h"

#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/FeasibilitySeparation.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/OptimalitySeparation.h"
#include "../milp/MILPMethodManager.h"

std::string RobustMethods::CCG_KKT_SOS1::description() const {
    return "Column-and-constraint generation with KKT-based separation using SOS1.";
}

std::vector<RobustMethod::Conditions> RobustMethods::CCG_KKT_SOS1::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_two_stage_problem = true;
        conditions.requires_continuous_second_stage = true;
        conditions.requires_decision_independent_uncertainty_set = true;
        conditions.requires_a_solver_with_sos1 = true;

        result.emplace_back(conditions);
    }

    return result;
}

void RobustMethods::CCG_KKT_SOS1::set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const {

    const auto& robust_description = t_manager.robust_description();
    const auto& bilevel_description = t_manager.bilevel_description();
    const auto& stage_analysis = t_manager.stage_analysis();
    const auto& args = t_manager.args();

    MILPAnalysisResult milp_analysis;
    milp_analysis.has_sos_constraints = true;

    const auto sub_milp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args, false, milp_analysis);

    auto ccg = idol::Robust::ColumnAndConstraintGeneration(robust_description, bilevel_description);
    ccg.with_initial_scenario_by_maximization(*sub_milp_optimizer);
    ccg.with_master_optimizer(*sub_milp_optimizer);

    auto kkt = idol::Bilevel::KKT();
    kkt.with_sos1_constraints(true);
    kkt.with_single_level_optimizer(*sub_milp_optimizer);

    if (!args.complete_recourse) {
        std::cout << "-- The problem is not known to have complete recourse, adding feasibility separation (use the --complete-recourse flag otherwise)." << std::endl;
        auto feasibility_separation = idol::Robust::CCG::FeasibilitySeparation();
        feasibility_separation.with_bilevel_optimizer(kkt);
        feasibility_separation.with_bounds_on_slack_variables(stage_analysis.second_stage.all_bounded);

        ccg.add_separation(feasibility_separation);
    }

    auto optimality_separation = idol::Robust::CCG::OptimalitySeparation();
    optimality_separation.with_bilevel_optimizer(kkt);

    ccg.add_separation(optimality_separation);

    t_model.use(ccg);
}
