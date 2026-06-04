//
// Created by Henri on 26/05/2026.
//
#include "BBBB.h"
#include "../milp/MILPMethodManager.h"
#include "idol/robust/optimizers/bilevel-based-branch-and-bound/MaxMinRelaxation.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/IntegerMaster.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/RENS.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/SimpleRounding.h"
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/CriticalValueColumnAndConstraintGeneration.h"

std::string RobustMethods::BBBB::description() const {
    return "Bilevel-based branch-and-bound.";
}

std::vector<RobustMethod::Conditions> RobustMethods::BBBB::conditions() {

    std::vector<Conditions> result;

    {
        Conditions conditions;
        conditions.requires_two_stage_problem = true;
        conditions.requires_decision_independent_uncertainty_set = true;
        conditions.requires_objective_uncertainty_only = true;
        conditions.requires_continuous_uncertainty_set = true;

        result.emplace_back(conditions);
    }

    {
        Conditions conditions;
        conditions.requires_two_stage_problem = true;
        conditions.requires_decision_independent_uncertainty_set = true;
        conditions.requires_binary_uncertainty = true;

        result.emplace_back(conditions);
    }

    return result;
}

void RobustMethods::BBBB::set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const {

    const auto& bilevel_description = t_manager.bilevel_description();
    const auto& robust_description = t_manager.robust_description();
    const auto& args = t_manager.args();

    const auto sub_milp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args);

    // Branching Candidates
    std::list<idol::Var> branching_candidates;
    for (const auto& var : t_model.vars()) {
        if (bilevel_description.is_upper(var) && t_model.get_var_type(var) != idol::Continuous) {
            branching_candidates.push_back(var);
        }
    }

    auto branch_and_bound = idol::BranchAndBound();
    branch_and_bound.with_branching_rule(idol::PseudoCost(branching_candidates.begin(), branching_candidates.end()));
    branch_and_bound.with_node_selection_rule(idol::BestBound());
    branch_and_bound.with_logs(true);
    branch_and_bound.with_logger(idol::Logs::BranchAndBound::Info().with_frequency_in_seconds(0).with_node_logs(false));

    auto max_min_relaxation = idol::Robust::MaxMinRelaxation(robust_description, bilevel_description);
    max_min_relaxation.with_master_optimizer(*sub_milp_optimizer);
    max_min_relaxation.with_deterministic_optimizer(*sub_milp_optimizer);
    max_min_relaxation.with_indicator(false);

    t_model.use(branch_and_bound + max_min_relaxation);
}
