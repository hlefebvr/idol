//
// Created by Henri on 20/05/2026.
//

#include "KKT_BigM.h"
#include "../milp/MILPMethodManager.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"

std::string BilevelMethods::KKT_BigM::description() const {
    return "Solves via the KKT-based single-level reformulation using user-provided Big-M values.";
}

std::vector<BilevelMethod::Conditions> BilevelMethods::KKT_BigM::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_continuous_follower = true;
        conditions.requires_big_M_file = true;

        result.emplace_back(conditions);
    }

    return result;
}

std::unique_ptr<idol::OptimizerFactory> BilevelMethods::KKT_BigM::get_optimizer_factory(const BilevelMethodManager& t_parent) const {

    std::unique_ptr<idol::OptimizerFactory> result;

    const auto& args = t_parent.args();

    const auto sub_milp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args);

    auto kkt = idol::Bilevel::KKT(t_parent.bilevel_description());
    kkt.with_single_level_optimizer(*sub_milp_optimizer);

    auto bound_provider = idol::Reformulators::KKT::BoundProviderMap::from_file(args.bound_provider);
    kkt.with_bound_provider(bound_provider);

    result.reset(kkt.clone());

    return result;
}
