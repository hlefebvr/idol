//
// Created by Henri on 20/05/2026.
//

#include "KKT_NLP.h"
#include "../milp/MILPMethodManager.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"

std::string BilevelMethods::KKT_NLP::description() const {
    return "Solves via the KKT-based single-level reformulation as an NLP.";
}

std::vector<BilevelMethod::Conditions> BilevelMethods::KKT_NLP::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_continuous_follower = true;

        result.emplace_back(conditions);
    }

    return result;
}

std::unique_ptr<idol::OptimizerFactory> BilevelMethods::KKT_NLP::get_optimizer_factory(const BilevelMethodManager& t_parent) const {

    std::unique_ptr<idol::OptimizerFactory> result;

    const auto& args = t_parent.args();

    MILPAnalysisResult milp_analysis;
    milp_analysis.has_quadratic_constraints = true;

    const auto sub_milp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args, false, milp_analysis);

    auto kkt = idol::Bilevel::KKT(t_parent.bilevel_description());
    kkt.with_single_level_optimizer(*sub_milp_optimizer);
    result.reset(kkt.clone());

    return result;
}
