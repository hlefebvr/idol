//
// Created by Henri on 19/05/2026.
//

#include "JuMP.h"
#include "idol/mixed-integer/optimizers/wrappers/JuMP/JuMP.h"

#include <string>
#include <vector>
#include <sstream>

std::vector<std::string> MILPMethods::JuMP::split_modules(const std::string& t_input) {
    std::vector<std::string> result;
    std::stringstream ss(t_input);
    std::string item;

    while (std::getline(ss, item, ',')) {
        // trim spaces
        item.erase(0, item.find_first_not_of(" \t\n\r"));
        item.erase(item.find_last_not_of(" \t\n\r") + 1);

        if (!item.empty()) {
            result.push_back(item);
        }
    }

    return result;
}

std::string MILPMethods::JuMP::description() const {
    return "Calls the Julia package JuMP [https://jump.dev/].";
}

std::vector<MILPMethod::Conditions> MILPMethods::JuMP::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_jump = true;
        conditions.requires_jump_optimizer = true;

        result.emplace_back(conditions) ;
    }

    return result;
}

std::unique_ptr<idol::OptimizerFactory> MILPMethods::JuMP::get_optimizer_factory(const MILPMethodManager& t_parent, bool t_continuous_relaxation) const {

    std::unique_ptr<idol::OptimizerFactory> result;

    const auto& args = t_parent.args();

    if (t_continuous_relaxation) {
        auto jump = idol::JuMP::ContinuousRelaxation(args.jump_optimizer);
        for (const auto& module : split_modules(args.julia_using)) {
            jump.with_julia_module(module);
        }
        result.reset(jump.clone());
    } else {
        auto jump = idol::JuMP(t_parent.args().jump_optimizer);
        for (const auto& module : split_modules(args.julia_using)) {
            jump.with_julia_module(module);
        }
        result.reset(jump.clone());
    }

    return result;
}
