//
// Created by Henri on 26/05/2026.
//
#include "BBBB.h"
#include "../milp/MILPMethodManager.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/CriticalValueColumnAndConstraintGeneration.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"


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

    return result;
}

void RobustMethods::BBBB::set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const {

    const auto& bilevel_description = t_manager.bilevel_description();
    const auto& robust_description = t_manager.robust_description();
    const auto& args = t_manager.args();

    const auto sub_milp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args);
    const auto sub_lp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args, true);

    // Branching Candidates
    std::list<idol::Var> branching_candidates;
    for (const auto& var : t_model.vars()) {
        if (bilevel_description.is_upper(var) && t_model.get_var_type(var) != idol::Continuous) {
            branching_candidates.push_back(var);
        }
    }

    // Lower bounding model
    auto hpr = robust_description.uncertainty_set().copy();

    auto uncertainty_set = t_model.copy();
    uncertainty_set.set_obj_expr(0.);
    const auto one = hpr.add_var(1, 1, idol::Continuous, 0, "__obj");

    idol::Robust::Description new_robust_description(uncertainty_set);
    for (const auto& [var, coeff] : t_model.get_obj_expr().affine().linear()) {
        new_robust_description.set_uncertain_obj(one, new_robust_description.uncertain_obj(one) - coeff * var);
    }
    for (const auto& [var, unc_coeff] : robust_description.uncertain_obj()) {
        for (const auto& [unc_var, coeff] : unc_coeff) {
            new_robust_description.set_uncertain_obj(unc_var, new_robust_description.uncertain_obj(unc_var) - coeff * var);
        }
    }

    auto branch_and_bound = idol::BranchAndBound();
    branch_and_bound.with_branching_rule(idol::PseudoCost(branching_candidates.begin(), branching_candidates.end()));
    branch_and_bound.with_node_selection_rule(idol::BestBound());

    auto cv_ccg = idol::Robust::CriticalValueColumnAndConstraintGeneration(new_robust_description);
    cv_ccg.with_master_optimizer(*sub_milp_optimizer);
    cv_ccg.with_deterministic_optimizer(*sub_milp_optimizer);

    hpr.use(branch_and_bound + cv_ccg);

    hpr.optimize();

}
