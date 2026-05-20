//
// Created by Henri on 20/05/2026.
//

#include "KKT_SOS1.h"
#include "../milp/MILPMethodManager.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"

std::string BilevelMethods::KKT_SOS1::description() const {
    return "Solves via the KKT-based single-level reformulation using SOS1 constraints.";
}

std::vector<BilevelMethod::Conditions> BilevelMethods::KKT_SOS1::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_continuous_follower = true;

        result.emplace_back(conditions);
    }

    return result;
}

std::unique_ptr<idol::OptimizerFactory> BilevelMethods::KKT_SOS1::get_optimizer_factory(const BilevelMethodManager& t_parent) const {

    std::unique_ptr<idol::OptimizerFactory> result;

    const auto& args = t_parent.args();

    MILPAnalysisResult milp_analysis;
    milp_analysis.has_sos_constraints = true;

    const auto sub_milp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args, false, milp_analysis);

    auto kkt = idol::Bilevel::KKT(t_parent.bilevel_description());
    kkt.with_single_level_optimizer(*sub_milp_optimizer);
    kkt.with_sos1_constraints(true);
    result.reset(kkt.clone());

    return result;
}
