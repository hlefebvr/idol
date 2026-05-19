//
// Created by Henri on 19/05/2026.
//

#include "CCG_CV.h"
#include "../milp/MILPMethodManager.h"
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/CriticalValueColumnAndConstraintGeneration.h"

std::string RobustMethods::CCG_CV::description() const {
    return "Critical-value column-and-constraint generation; see Lozano and Borrero (2025) [https://link.springer.com/article/10.1007/s10107-025-02249-6].";
}

std::vector<RobustMethod::Conditions> RobustMethods::CCG_CV::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_single_stage_problem = true;
        conditions.requires_general_integer_uncertainty_set = true;
        conditions.requires_uncertainty_set_with_integer_coefficients = true;
        conditions.requires_decision_dependent_uncertainty_set = true;

        result.emplace_back(conditions);
    }

    return result;
}

void RobustMethods::CCG_CV::set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const {

    const auto& robust_description = t_manager.robust_description();
    const auto& args = t_manager.args();

    const auto sub_milp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args);

    auto ccg = idol::Robust::CriticalValueColumnAndConstraintGeneration(robust_description);
    ccg.with_master_optimizer(*sub_milp_optimizer);
    ccg.with_deterministic_optimizer(*sub_milp_optimizer);

    t_model.use(ccg);

}
