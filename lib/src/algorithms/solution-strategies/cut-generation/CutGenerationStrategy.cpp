//
// Created by henri on 05/10/22.
//
#include <iomanip>
#include "algorithms/solution-strategies/cut-generation/CutGenerationStrategy.h"

CutGenerationStrategy::CutGenerationStrategy(DecompositionId &&t_id) : AbstractGenerationStrategy(std::move(t_id)) {

}

void CutGenerationStrategy::build() {
    for (auto& ptr_to_subproblem : m_subproblems) {
        ptr_to_subproblem->build();
    }
}

void CutGenerationStrategy::solve() {

    initialize();

    while (!is_terminated()) {

        rmp_solution_strategy().solve();

        save_last_rmp_primal_solution();

        log_last_rmp_primal_solution();

        analyze_last_rmp_primal_solution();

        update_subproblems();

        solve_subproblems();

        add_cuts();

    }

}

void CutGenerationStrategy::initialize() {
    m_is_terminated = false;
}

void CutGenerationStrategy::save_last_rmp_primal_solution() {
    m_last_rmp_primals = std::make_unique<Solution::Primal>(rmp_solution_strategy().primal_solution());
}

void CutGenerationStrategy::analyze_last_rmp_primal_solution() {

    if (rmp_is_infeasible()) {
        terminate_for_rmp_is_infeasible();
        return;
    }

    if (rmp_is_unbounded()) {
        throw Exception("Did not know what to do with unbounded RMP.");
    }

    if (rmp_could_not_be_solved_to_optimality()) {
        terminate_for_rmp_could_not_be_solved_to_optimality();
    }

    EASY_LOG(Trace, "cut-generation", "Using primal solution for separation.");

}

bool CutGenerationStrategy::rmp_is_unbounded() const {
    return m_last_rmp_primals->status() == Unbounded;
}

bool CutGenerationStrategy::rmp_is_infeasible() const {
    return m_last_rmp_primals->status() == Infeasible;
}

bool CutGenerationStrategy::rmp_could_not_be_solved_to_optimality() const {
    return m_last_rmp_primals->status() != Optimal;
}

void CutGenerationStrategy::terminate_for_rmp_is_infeasible() {
    EASY_LOG(Trace, "cut-generation", "Terminate. Infeasible RMP.");
    terminate();
}

void CutGenerationStrategy::terminate_for_rmp_could_not_be_solved_to_optimality() {
    EASY_LOG(Trace, "cut-generation", "Terminate. RMP returned with primal status \"" << m_last_rmp_primals->status() << "\".");
    terminate();
}

void CutGenerationStrategy::terminate() {
    m_is_terminated = true;
}

void CutGenerationStrategy::update_subproblems() {
    if (is_terminated()) { return; }

    for (auto& ptr_to_subproblem : m_subproblems) {
        auto row = ptr_to_subproblem->get_separation_objective(*m_last_rmp_primals);
        EASY_LOG(Trace, "cut-generation", "Setting separation objective to " << row);
        ptr_to_subproblem->update_separation_objective(row);
    }
}

void CutGenerationStrategy::solve_subproblems() {

    if (is_terminated()) { return; }

    for (auto& ptr_to_subproblem : m_subproblems) {

        ptr_to_subproblem->solve();

        ptr_to_subproblem->save_last_primal_solution();

        ptr_to_subproblem->log_last_primal_solution();

        analyze_last_subproblem_primal_solution(*ptr_to_subproblem);

        if (is_terminated()) { break; }

    }

}

void CutGenerationStrategy::add_cuts() {

    if (is_terminated()) { return; }

    bool improving_columns_found = false;

    for (auto& ptr_to_subproblem : m_subproblems) {

        if (ptr_to_subproblem->violated_cut_found()) {
            improving_columns_found = true;
            ptr_to_subproblem->add_cut_to_rmp();
        }

    }

    if (!improving_columns_found) {
        terminate_for_no_violated_cut_found();
    }

}

void CutGenerationStrategy::terminate_for_no_violated_cut_found() {
    EASY_LOG(Trace, "cut-generation", "Terminate. No violated cut found.");
    terminate();
}

void
CutGenerationStrategy::analyze_last_subproblem_primal_solution(const AbstractCutGenerationSubproblem &t_subproblem) {

    if (t_subproblem.is_unbounded()) {
        throw Exception("Did not know what to do with unbounded SP.");
        return;
    }

    if (t_subproblem.is_infeasible()) {
        terminate_for_subproblem_is_infeasible();
        return;
    }

    if (t_subproblem.could_not_be_solved_to_optimality()) {
        terminate_for_subproblem_could_not_be_solved_to_optimality();
        return;
    }

}

void CutGenerationStrategy::terminate_for_subproblem_is_infeasible() {
    EASY_LOG(Trace, "cut-generation", "Terminate. Infeasible SP.");
    terminate();
}

void CutGenerationStrategy::terminate_for_subproblem_could_not_be_solved_to_optimality() {
    EASY_LOG(Trace, "cut-generation", "Terminate. SP could not be solved to optimality using the provided exact method."
                                         "Reported status: ...............");
    terminate();
}

void CutGenerationStrategy::log_last_rmp_primal_solution() const {
    EASY_LOG(Debug, "cut-generation",
             std::setw(5)
             << "RMP"
             << std::setw(15)
             << m_last_rmp_primals->status()
             << std::setw(10)
             << m_last_rmp_primals->objective_value()
    );
}

Solution::Primal CutGenerationStrategy::primal_solution() const {
    Solution::Primal result = rmp_solution_strategy().primal_solution();
    for (auto& ptr_to_subproblem : m_subproblems) {
        result.merge_without_conflict(ptr_to_subproblem->primal_solution());
    }
    return result;
}
