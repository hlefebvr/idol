//
// Created by Henri on 19/05/2026.
//

#include "JuMP.h"
#include "idol/mixed-integer/optimizers/wrappers/JuMP/JuMP.h"

std::string MILPMethods::JuMP::description() const {
    return "Calls the Julia package JuMP [https://jump.dev/]";
}

std::vector<MILPMethod::Conditions> MILPMethods::JuMP::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_jump = true;
        conditions.requires_jump_optimizer = true;

        result.emplace_back(conditions);
    }

    return result;
}

std::unique_ptr<idol::OptimizerFactory> MILPMethods::JuMP::get_optimizer_factory(const MILPMethodManager& t_parent, bool t_continuous_relaxation) const {

    std::unique_ptr<idol::OptimizerFactory> result;

    if (t_continuous_relaxation) {
        auto jump = idol::JuMP::ContinuousRelaxation(t_parent.args().jump_optimizer);
        result.reset(jump.clone());
    } else {
        auto jump = idol::JuMP(t_parent.args().jump_optimizer);
        result.reset(jump.clone());
    }

    return result;
}
