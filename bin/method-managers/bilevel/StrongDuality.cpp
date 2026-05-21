//
// Created by Henri on 20/05/2026.
//

#include "StrongDuality.h"
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"
#include "../milp/MILPMethodManager.h"

std::string BilevelMethods::StrongDuality::description() const {
    return "Solves via the strong-duality-based single-level reformulation as an NLP.";
}

std::vector<BilevelMethod::Conditions> BilevelMethods::StrongDuality::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_continuous_follower = true;

        result.emplace_back(conditions);
    }

    return result;
}

std::unique_ptr<idol::OptimizerFactory> BilevelMethods::StrongDuality::get_optimizer_factory(const BilevelMethodManager& t_parent) const {

    std::unique_ptr<idol::OptimizerFactory> result;

    const auto& args = t_parent.args();

    MILPAnalysisResult milp_analysis;
    milp_analysis.has_quadratic_constraints = true;

    const auto sub_milp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args, false, milp_analysis);

    auto strong_duality = idol::Bilevel::StrongDuality(t_parent.bilevel_description());
    strong_duality.with_single_level_optimizer(*sub_milp_optimizer);
    result.reset(strong_duality.clone());

    return result;
}
