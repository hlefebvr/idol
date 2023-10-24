//
// Created by henri on 18.10.23.
//
#include "idol/optimizers/branch-and-bound/nodes/NodeVarInfo.h"
#include "idol/optimizers/branch-and-bound/Optimizers_BranchAndBound.h"

void idol::NodeVarInfo::set_local_upper_bound(const idol::Var &t_var, double t_ub) {
    m_branching_decision = std::make_optional<BranchingDecision>(t_var, LessOrEqual, t_ub);
}

void idol::NodeVarInfo::set_local_lower_bound(const idol::Var &t_var, double t_lb) {
    m_branching_decision = std::make_optional<BranchingDecision>(t_var, GreaterOrEqual, t_lb);
}
idol::NodeVarUpdator<idol::NodeVarInfo> *
idol::NodeVarInfo::create_updator(idol::Model &t_relaxation) {
    return new NodeVarUpdator<NodeVarInfo>(t_relaxation);
}

void idol::NodeVarInfo::save(const Optimizers::BranchAndBound<NodeVarInfo>& t_parent,
                             const idol::Model &t_original_formulation,
                             const idol::Model &t_model) {

    const auto status = t_model.get_status();
    const auto reason = t_model.get_reason();

    m_primal_solution.set_status(status);
    m_primal_solution.set_reason(reason);

    if (status == Infeasible) {
        m_primal_solution.set_objective_value(+Inf);
        return;
    }

    if (status == Unbounded) {
        m_primal_solution.set_objective_value(-Inf);
        return;
    }

    if (status == Fail || status == SubOptimal) {
        m_primal_solution.set_objective_value(t_original_formulation.get_best_bound());
        m_sum_of_infeasibilities = Inf;
        return;
    }

    if (status != Optimal && status != Feasible) {
        return;
    }

    m_primal_solution = save_primal(t_original_formulation, t_model);

    m_sum_of_infeasibilities = 0;
    for (const auto& [var, value] : m_primal_solution) {
        m_sum_of_infeasibilities.value() += std::abs(value - std::round(value));
    }

}

idol::NodeVarInfo *idol::NodeVarInfo::create_child() const {
    return new NodeVarInfo();
}
