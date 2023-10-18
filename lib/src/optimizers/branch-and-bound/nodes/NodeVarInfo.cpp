//
// Created by henri on 18.10.23.
//
#include "optimizers/branch-and-bound/nodes/NodeVarInfo.h"

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

void idol::NodeVarInfo::save(const idol::Model &t_original_formulation,
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

    if (status != Optimal && status != Feasible && status != SubOptimal) {
        return;
    }

    m_primal_solution = save_primal(t_original_formulation, t_model);

}

idol::NodeVarInfo *idol::NodeVarInfo::create_child() const {
    return new NodeVarInfo(*this);
}
