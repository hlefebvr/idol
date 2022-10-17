//
// Created by henri on 17/10/22.
//
#include "../../../include/algorithms/branch-and-bound/BranchAndBound.h"

void BranchAndBound::Callback::AdvancedContext::fix_variables(const std::list<std::pair<Var, double>> &t_fixations) {

    if (!m_temp_fixations.empty()) {
        throw Exception("Variables have already been fixed.");
    }

    for (const auto& [var, value] : t_fixations) {
        m_temp_fixations.emplace_back(var, var.lb(), var.ub());
        m_parent.m_solution_strategy->update_lb(var, value);
        m_parent.m_solution_strategy->update_ub(var, value);
    }
}

void BranchAndBound::Callback::AdvancedContext::resolve() {
    m_parent.m_solution_strategy->solve();
}

BranchAndBound::Callback::AdvancedContext::~AdvancedContext() {

    for (const auto& [var, lb, ub] : m_temp_fixations) {
        m_parent.m_solution_strategy->update_lb(var, lb);
        m_parent.m_solution_strategy->update_ub(var, ub);
    }

}

Solution::Primal BranchAndBound::Callback::AdvancedContext::node_primal_solution() const {
    return m_parent.current_node().primal_solution();
}

Solution::Primal BranchAndBound::Callback::AdvancedContext::primal_solution() const {
    return m_parent.m_solution_strategy->primal_solution();
}

bool BranchAndBound::Callback::AdvancedContext::submit_solution(Solution::Primal &&t_solution) {
    return m_parent.submit_solution(std::move(t_solution));
}

BranchAndBound::Callback::AdvancedContext &BranchAndBound::Callback::advanced(Callback::Context &t_ctx) {
    auto ptr = dynamic_cast<AdvancedContext*>(&t_ctx);
    assert(ptr);
    return *ptr;
}

const BranchAndBound::Callback::AdvancedContext &BranchAndBound::Callback::advanced(const Callback::Context &t_ctx) const {
    auto ptr = dynamic_cast<const AdvancedContext*>(&t_ctx);
    assert(ptr);
    return *ptr;
}