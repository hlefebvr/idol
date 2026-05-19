//
// Created by Henri on 19/05/2026.
//

#include "CCG_Farkas.h"

#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/BigMFreeSeparation.h"
#include "../milp/MILPMethodManager.h"

std::string RobustMethods::CCG_Farkas::description() const {
    return "Column-and-constraint generation with Farkas-based separation; see Ayoub and Poss (2016) [https://doi.org/10.1007/s10287-016-0249-2].";
}

std::vector<RobustMethod::Conditions> RobustMethods::CCG_Farkas::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_two_stage_problem = true;
        conditions.requires_binary_uncertainty = true;
        conditions.requires_continuous_second_stage = true;
        conditions.requires_decision_independent_uncertainty_set = true;

        result.emplace_back(conditions);
    }

    {
        Conditions conditions;
        conditions.requires_two_stage_problem = true;
        conditions.requires_a_0_1_uncertainty_set = true;
        conditions.requires_continuous_second_stage = true;
        conditions.requires_decision_independent_uncertainty_set = true;

        result.emplace_back(conditions);
    }

    return result;
}

void RobustMethods::CCG_Farkas::set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const {

    const auto& robust_description = t_manager.robust_description();
    const auto& bilevel_description = t_manager.bilevel_description();
    const auto& uncertainty_analysis = t_manager.uncertainty_analysis();
    const auto& args = t_manager.args();

    const auto sub_milp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args);

    auto ccg = idol::Robust::ColumnAndConstraintGeneration(robust_description, bilevel_description);
    ccg.with_initial_scenario_by_maximization(*sub_milp_optimizer);
    ccg.with_master_optimizer(*sub_milp_optimizer);

    auto farkas = idol::Robust::CCG::BigMFreeSeparation();
    farkas.with_single_level_optimizer(*sub_milp_optimizer);
    farkas.with_binary_uncertainty_set(!uncertainty_analysis.has_continuous || uncertainty_analysis.is_zero_one_polytope);

    ccg.add_separation(farkas);

    t_model.use(ccg);
}
