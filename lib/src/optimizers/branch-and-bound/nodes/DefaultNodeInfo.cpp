//
// Created by henri on 18.10.23.
//
#include "idol/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"
#include "idol/optimizers/branch-and-bound/Optimizers_BranchAndBound.h"

idol::DefaultNodeUpdator<idol::DefaultNodeInfo> *
idol::DefaultNodeInfo::create_updator(idol::Model &t_relaxation) {
    return new DefaultNodeUpdator<DefaultNodeInfo>(t_relaxation);
}

void idol::DefaultNodeInfo::add_branching_constraint(const Ctr &t_ctr, TempCtr t_temporary_constraint) {
    m_constraint_branching_decisions.emplace_back(t_ctr, std::move(t_temporary_constraint));
}

void idol::DefaultNodeInfo::add_branching_variable(const idol::Var &t_var, idol::CtrType t_type, double t_bound) {
    m_variable_branching_decisions.emplace_back(t_var, t_type, t_bound);
}

void idol::DefaultNodeInfo::save(const idol::Model &t_original_formulation,
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

idol::DefaultNodeInfo *idol::DefaultNodeInfo::create_child() const {
    return new DefaultNodeInfo();
}
