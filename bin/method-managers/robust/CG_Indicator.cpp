//
// Created by Henri on 19/05/2026.
//

#include "CG_Indicator.h"
#include "../MILPMethodManager.h"
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/CriticalValueColumnAndConstraintGeneration.h"

std::string RobustMethods::CG_Indicator::description() const {
    return "Scenario generation (indicator functions in case of decision-dependent uncertainty sets)";
}

std::vector<RobustMethod::Conditions> RobustMethods::CG_Indicator::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_single_stage_problem = true;
        conditions.requires_binary_linking_variables_in_uncertainty_set = true;
        conditions.requires_decision_dependent_uncertainty_set = true;

        result.emplace_back(conditions);
    }

    {
        Conditions conditions;
        conditions.requires_single_stage_problem = true;
        conditions.requires_decision_independent_uncertainty_set = true;

        result.emplace_back(conditions);
    }

    return result;
}

void RobustMethods::CG_Indicator::set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const {

    const auto& robust_description = t_manager.robust_description();
    const auto& args = t_manager.args();

    MILPMethodManager sub_milp_method_manager;
    const auto sub_milp_optimizer = sub_milp_method_manager.get_sub_milp_optimizer(args);

    auto ccg = idol::Robust::CriticalValueColumnAndConstraintGeneration(robust_description);
    ccg.with_master_optimizer(*sub_milp_optimizer);
    ccg.with_deterministic_optimizer(*sub_milp_optimizer);
    ccg.with_indicator(true);

    t_model.use(ccg);

}
