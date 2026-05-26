//
// Created by Henri on 22/05/2026.
//

#include "Convexification.h"
#include "../milp/MILPMethodManager.h"
#include "idol/mixed-integer/modeling/models/KKT.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/callbacks/ReducedCostFixing.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/IntegerMaster.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/Neame.h"
#include "idol/mixed-integer/optimizers/presolve/BoundRounding.h"
#include "idol/mixed-integer/optimizers/presolve/OneRowBoundTightening.h"
#include "idol/robust/optimizers/convexification/Convexification.h"
#include "idol/robust/optimizers/wrappers/ROCPP.h"

std::string RobustMethods::Convexification::description() const {
    return "Branch-and-Price algorithm for objective uncertainty from Arslan et al. (2021); [https://doi.org/10.1287/ijoc.2021.1061].";
}

std::vector<RobustMethod::Conditions> RobustMethods::Convexification::conditions() {

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

void RobustMethods::Convexification::set_optimizer(idol::Model& t_model, const RobustMethodManager& t_manager) const {

    const auto& bilevel_description = t_manager.bilevel_description();
    const auto& robust_description = t_manager.robust_description();
    const auto& args = t_manager.args();

    const auto sub_milp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args);
    const auto sub_lp_optimizer = MILPMethodManager::get_sub_milp_optimizer(args, true);

    auto convexification = idol::Robust::Convexification(robust_description, bilevel_description);
    convexification.with_master_optimizer(*sub_lp_optimizer);
    convexification.with_sub_problem_optimizer(*sub_milp_optimizer);
    convexification.with_integer_master_heuristic_optimizer(*sub_milp_optimizer);

    t_model.use(convexification);

}
