//
// Created by Henri on 20/05/2026.
//

#include "PADM.h"
#include "idol/bilevel/optimizers/PADM/PADM.h"

#include "../milp/MILPMethodManager.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"

std::string BilevelMethods::PADM::description() const {
    return "Computes a feasible point using a penalty alternating direction method.";
}

std::vector<BilevelMethod::Conditions> BilevelMethods::PADM::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_continuous_follower = true;

        result.emplace_back(conditions);
    }

    return result;
}

std::unique_ptr<idol::OptimizerFactory> BilevelMethods::PADM::get_optimizer_factory(const BilevelMethodManager& t_parent) const {

    std::unique_ptr<idol::OptimizerFactory> result;

    const auto& args = t_parent.args();

    const auto sub_milp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args);

    auto kkt = idol::Bilevel::PADM(t_parent.bilevel_description());
    kkt.with_single_level_optimizer(*sub_milp_optimizer);
    result.reset(kkt.clone());

    return result;
}
