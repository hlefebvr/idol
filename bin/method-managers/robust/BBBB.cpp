//
// Created by Henri on 26/05/2026.
//
#include "BBBB.h"
#include "../milp/MILPMethodManager.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/robust/optimizers/bilevel-based-branch-and-bound/MaxMinRelaxation.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/IntegerMaster.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/RENS.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/SimpleRounding.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/robust/optimizers/bilevel-based-branch-and-bound/Optimizers_MaxMinRelaxation.h"
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/OptimalitySeparation.h"
#include "idol/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/CriticalValueColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/Optimizers_CriticalValueColumnAndConstraintGeneration.h"

template<class NodeInfoT = idol::DefaultNodeInfo>
class EvaluateCallback : public idol::BranchAndBoundCallbackFactory<NodeInfoT> {
public:
    class Strategy : public idol::BranchAndBoundCallback<NodeInfoT> {
        const unsigned int m_max_evaluations = 10;
        const unsigned int m_node_frequency = 20;
    protected:
        void operator()(idol::CallbackEvent t_event) override;
    };

    idol::BranchAndBoundCallback<NodeInfoT>* operator()() override { return new Strategy(); }
    [[nodiscard]] idol::BranchAndBoundCallbackFactory<NodeInfoT>* clone() const override { return new EvaluateCallback<NodeInfoT>(*this); }
};

template <class NodeInfoT>
void EvaluateCallback<NodeInfoT>::Strategy::operator()(idol::CallbackEvent t_event) {

    if (t_event != idol::InvalidSolution) {
        return;
    }

    if (this->node_count() % m_node_frequency != 0) {
        return;
    }

    const auto& max_min_relaxation = this->relaxation().optimizer().template as<idol::Optimizers::Robust::MaxMinRelaxation>();
    const auto& cvccg = max_min_relaxation.get_formulation().model.optimizer().template as<idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration>();
    const auto& branching_candidates = cvccg.branching_candidates();

    auto relaxation_fixed = this->relaxation().copy();
    relaxation_fixed.optimizer().set_param_time_limit(1e-2);
    relaxation_fixed.optimize();

    unsigned int n_evaluations = 0;
    for (const auto& uncertainty : cvccg.get_formulation().uncertainties()) {

        std::vector<idol::CVCCG::Formulation::CurrentlyPresentCut> cuts;
        std::copy(uncertainty.currently_present_cuts().begin(), uncertainty.currently_present_cuts().end(), std::back_inserter(cuts));
        std::sort(cuts.begin(), cuts.end(), [](const auto& t_a, const auto& t_b) {
            return t_a.scenario->scenario.objective_value() < t_b.scenario->scenario.objective_value();
        });

        for (const auto& cut : cuts) {

            for (const auto& var : branching_candidates) {
                const double val = cut.scenario->scenario.get(var);
                relaxation_fixed.set_var_lb(var, val);
                relaxation_fixed.set_var_ub(var, val);
            }
            const double remaining_time = this->original_model().optimizer().get_remaining_time();
            relaxation_fixed.optimizer().set_param_time_limit(remaining_time);
            relaxation_fixed.optimize();

            const auto status = relaxation_fixed.get_status();
            if (status == idol::Optimal || status == idol::Feasible) {
                auto* info = new idol::DefaultNodeInfo();
                info->set_primal_solution(idol::save_primal(relaxation_fixed));
                this->submit_heuristic_solution(info);
            }

            n_evaluations++;
            if (n_evaluations >= m_max_evaluations) {
                return;
            }
        }
    }

}

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

    for (const auto& var : t_model.vars()) {
        if (t_model.get_var_type(var) == idol::Continuous) {
            t_model.set_var_type(var, idol::Integer);
        }
    }

    std::list<idol::PrimalPoint> initial_scenarios;

    if (false) {

        auto ccg = idol::Robust::ColumnAndConstraintGeneration(robust_description, bilevel_description);
        ccg.with_master_optimizer(idol::Gurobi());
        ccg.add_separation(idol::Robust::CCG::OptimalitySeparation().with_bilevel_optimizer(idol::Bilevel::MibS()));
        ccg.with_logs(true);
        ccg.with_iteration_limit(2);

        t_model.use(ccg);
        t_model.optimize();

        const auto& optimizer = t_model.optimizer().as<idol::Optimizers::Robust::ColumnAndConstraintGeneration>();
        for (const auto& scenario : optimizer.get_formulation().generated_scenarios()) {
            initial_scenarios.emplace_back(scenario);
        }

        std::cout << "Initializing with " << initial_scenarios.size() << " scenarios." << std::endl;

    }

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

    branch_and_bound.add_callback(EvaluateCallback());

    auto max_min_relaxation = idol::Robust::MaxMinRelaxation(robust_description, bilevel_description);
    max_min_relaxation.with_master_optimizer(*sub_milp_optimizer);
    max_min_relaxation.with_deterministic_optimizer(*sub_milp_optimizer);
    max_min_relaxation.with_indicator(false);

    for (const auto& scenario : initial_scenarios) {
        max_min_relaxation.add_initial_scenario(scenario);
    }

    t_model.use(branch_and_bound + max_min_relaxation);
}
