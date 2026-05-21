//
// Created by Henri on 19/05/2026.
//

#include "HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h"

std::string MILPMethods::HiGHS::description() const {
    return "High Performance Software for Linear Optimization [https://highs.dev/]";
}

std::vector<MILPMethod::Conditions> MILPMethods::HiGHS::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_highs = true;
        conditions.requires_no_sos_constraint = true;
        conditions.requires_no_quadratic_constraint = true;
        conditions.requires_no_quadratic_objective_function = true;

        result.emplace_back(conditions);
    }

    return result;
}

std::unique_ptr<idol::OptimizerFactory> MILPMethods::HiGHS::get_optimizer_factory(const MILPMethodManager& t_parent, bool t_continuous_relaxation) const {

    std::unique_ptr<idol::OptimizerFactory> result;

    auto highs = idol::HiGHS();
    highs.with_continuous_relaxation_only(t_continuous_relaxation);

    result.reset(highs.clone());

    return result;
}
