//
// Created by Henri on 21/05/2026.
//

#include "ROCPP_KAdaptability.h"
#include "../milp/MILPMethodManager.h"
#include "idol/robust/optimizers/wrappers/ROCPP.h"

std::string RobustMethods::ROCPP_KAdaptability::description() const {
    return "Approximates a two-stage robust problem by k-adaptability via ROC++.";
}

std::vector<RobustMethod::Conditions> RobustMethods::ROCPP_KAdaptability::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_two_stage_problem = true;
        conditions.requires_decision_independent_uncertainty_set = true;
        conditions.requires_binary_second_stage = true;
        conditions.requires_continuous_uncertainty_set = true;
        conditions.requires_rocpp = true;

        result.emplace_back(conditions);
    }

    return result;
}

void RobustMethods::ROCPP_KAdaptability::set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const {

    const auto& bilevel_description = t_manager.bilevel_description();
    const auto& robust_description = t_manager.robust_description();
    const auto& args = t_manager.args();

    const auto sub_milp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args);

    auto rocpp = idol::Robust::ROCPP(robust_description, bilevel_description, idol::Robust::ROCPP::KAdaptability);
    rocpp.with_deterministic_optimizer(*sub_milp_optimizer);

    t_model.use(rocpp);

}
