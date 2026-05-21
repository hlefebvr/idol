//
// Created by Henri on 19/05/2026.
//

#include "Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

std::string MILPMethods::Gurobi::description() const {
    return "Gurobi Optimizer [https://www.gurobi.com/]";
}

std::vector<MILPMethod::Conditions> MILPMethods::Gurobi::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_gurobi = true;

        result.emplace_back(conditions);
    }

    return result;
}

std::unique_ptr<idol::OptimizerFactory> MILPMethods::Gurobi::get_optimizer_factory(const MILPMethodManager& t_parent, bool t_continuous_relaxation) const {

    std::unique_ptr<idol::OptimizerFactory> result;

    auto gurobi = idol::Gurobi();
    gurobi.with_continuous_relaxation_only(t_continuous_relaxation);

    result.reset(gurobi.clone());

    return result;
}
