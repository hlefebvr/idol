//
// Created by henri on 05/01/23.
//
#include "algorithms/benders/Benders.h"
#include "algorithms/branch-and-bound/BranchAndBound_Events.h"

Benders::Benders(Model &t_model, const UserAttr &t_subproblem_flag)
    : m_reformulation(t_model, t_subproblem_flag) {

    const unsigned int n_subproblems = m_reformulation.subproblems().size();
    m_subproblems.reserve(n_subproblems);
    for (unsigned int i = 1 ; i <= n_subproblems ; ++i) {
        m_subproblems.emplace_back(*this, i);
    }

}

void Benders::initialize() {


    if (!m_master_solution_strategy) {
        throw Exception("No solution strategy at hand for solving master problem.");
    }

    for (auto& subproblem : m_subproblems) {
        subproblem.initialize();
    }

}

void Benders::execute() {

    initialize();

    m_master_solution_strategy->solve();

}

Solution::Primal Benders::primal_solution() const {
    return m_master_solution_strategy->primal_solution();
}
