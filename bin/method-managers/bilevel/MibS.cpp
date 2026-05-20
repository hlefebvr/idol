//
// Created by Henri on 20/05/2026.
//

#include "MibS.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"

std::string BilevelMethods::MibS::description() const {
    return "MibS [https://github.com/coin-or/MibS].";
}

std::vector<BilevelMethod::Conditions> BilevelMethods::MibS::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_MibS = true;
        conditions.requires_integer_linking_variables = true;

        result.emplace_back(conditions);
    }

    return result;
}

std::unique_ptr<idol::OptimizerFactory> BilevelMethods::MibS::get_optimizer_factory(const BilevelMethodManager& t_parent) const {

    std::unique_ptr<idol::OptimizerFactory> result;

    auto mibs = idol::Bilevel::MibS(t_parent.bilevel_description());
    result.reset(mibs.clone());

    return result;
}
