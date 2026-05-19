//
// Created by Henri on 19/05/2026.
//

#include "CCG_MibS.h"
#include "../MILPMethodManager.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/FeasibilitySeparation.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/OptimalitySeparation.h"

std::string RobustMethods::CCG_MibS::description() const {
    return "Column-and-constraint generation with separation by the mixed-integer bilevel solver MibS.";
}

std::vector<RobustMethod::Conditions> RobustMethods::CCG_MibS::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_two_stage_problem = true;
        conditions.requires_general_integer_uncertainty_set = true;
        conditions.requires_decision_independent_uncertainty_set = true;
        conditions.requires_mibs = true;
        conditions.requires_complete_recourse = true;

        result.emplace_back(conditions);
    }

    {
        Conditions conditions;
        conditions.requires_two_stage_problem = true;
        conditions.requires_general_integer_uncertainty_set = true;
        conditions.requires_decision_independent_uncertainty_set = true;
        conditions.requires_mibs = true;
        conditions.requires_bounded_second_stage = true;

        result.emplace_back(conditions);
    }

    return result;
}

void RobustMethods::CCG_MibS::set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const {

    const auto& robust_description = t_manager.robust_description();
    const auto& bilevel_description = t_manager.bilevel_description();
    const auto& stage_analysis = t_manager.stage_analysis();
    const auto& args = t_manager.args();

    MILPMethodManager sub_milp_method_manager;
    const auto sub_milp_optimizer = sub_milp_method_manager.get_sub_milp_optimizer(args);

    auto ccg = idol::Robust::ColumnAndConstraintGeneration(robust_description, bilevel_description);
    ccg.with_initial_scenario_by_maximization(*sub_milp_optimizer);
    ccg.with_master_optimizer(*sub_milp_optimizer);

    auto mibs = idol::Bilevel::MibS();
    mibs.with_feasibility_checker(*sub_milp_optimizer);

    if (!args.complete_recourse) {
        std::cout << "-- The problem is not known to have complete recourse, adding feasibility separation (use the --complete-recourse flag otherwise)." << std::endl;
        auto feasibility_separation = idol::Robust::CCG::FeasibilitySeparation();
        feasibility_separation.with_bilevel_optimizer(mibs);
        feasibility_separation.with_integer_slack_variables(true);

        ccg.add_separation(feasibility_separation);
    }

    auto optimality_separation = idol::Robust::CCG::OptimalitySeparation();
    optimality_separation.with_bilevel_optimizer(mibs);

    ccg.add_separation(optimality_separation);

    t_model.use(ccg);
}
