//
// Created by henri on 05/10/22.
//
#include <iomanip>
#include "algorithms/cut-generation/CutGeneration.h"

CutGeneration::CutGeneration(DecompositionId &&t_id) : GenerationAlgorithm(std::move(t_id)){

}

void CutGeneration::build() {
    for (auto& subproblem : m_subproblems) {
        subproblem.build();
    }
}

void CutGeneration::solve() {

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

void CutGeneration::initialize() {
    m_is_terminated = false;
}

void CutGeneration::save_last_rmp_primal_solution() {
    m_last_rmp_primals = std::make_unique<Solution::Primal>(rmp_solution_strategy().primal_solution());
}

void CutGeneration::analyze_last_rmp_primal_solution() {

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

bool CutGeneration::rmp_is_unbounded() const {
    return m_last_rmp_primals->status() == Unbounded;
}

bool CutGeneration::rmp_is_infeasible() const {
    return m_last_rmp_primals->status() == Infeasible;
}

bool CutGeneration::rmp_could_not_be_solved_to_optimality() const {
    return m_last_rmp_primals->status() != Optimal;
}

void CutGeneration::terminate_for_rmp_is_infeasible() {
    EASY_LOG(Trace, "cut-generation", "Terminate. Infeasible RMP.");
    terminate();
}

void CutGeneration::terminate_for_rmp_could_not_be_solved_to_optimality() {
    EASY_LOG(Trace, "cut-generation", "Terminate. RMP returned with primal status \"" << m_last_rmp_primals->status() << "\".");
    terminate();
}

void CutGeneration::terminate() {
    m_is_terminated = true;
}

void CutGeneration::update_subproblems() {
    if (is_terminated()) { return; }

    for (auto& subproblem : m_subproblems) {
        auto row = subproblem.get_separation_objective(*m_last_rmp_primals);
        EASY_LOG(Trace, "cut-generation", "Setting separation objective to " << row);
        subproblem.update_separation_objective(row);
    }
}

void CutGeneration::solve_subproblems() {

    if (is_terminated()) { return; }

    for (auto& subproblem : m_subproblems) {

        subproblem.solve();

        subproblem.save_last_primal_solution();

        subproblem.log_last_primal_solution();

        analyze_last_subproblem_primal_solution(subproblem);

        if (is_terminated()) { break; }

    }

}

void CutGeneration::add_cuts() {

    if (is_terminated()) { return; }

    bool improving_columns_found = false;

    for (auto& subproblem : m_subproblems) {

        if (subproblem.violated_cut_found()) {
            improving_columns_found = true;
            subproblem.add_cut_to_rmp();
        }

    }

    if (!improving_columns_found) {
        terminate_for_no_violated_cut_found();
    }

}

void CutGeneration::terminate_for_no_violated_cut_found() {
    EASY_LOG(Trace, "cut-generation", "Terminate. No violated cut found.");
    terminate();
}

void
CutGeneration::analyze_last_subproblem_primal_solution(const CutGenerationSubproblem &t_subproblem) {

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

void CutGeneration::terminate_for_subproblem_is_infeasible() {
    EASY_LOG(Trace, "cut-generation", "Terminate. Infeasible SP.");
    terminate();
}

void CutGeneration::terminate_for_subproblem_could_not_be_solved_to_optimality() {
    EASY_LOG(Trace, "cut-generation", "Terminate. SP could not be solved to optimality using the provided exact method."
                                         "Reported status: ...............");
    terminate();
}

void CutGeneration::log_last_rmp_primal_solution() const {
    EASY_LOG(Debug, "cut-generation",
             std::setw(5)
             << "RMP"
             << std::setw(15)
             << m_last_rmp_primals->status()
             << std::setw(10)
             << m_last_rmp_primals->objective_value()
    );
}

Solution::Primal CutGeneration::primal_solution() const {
    Solution::Primal result = rmp_solution_strategy().primal_solution();
    for (auto& subproblem : m_subproblems) {
        result.merge_without_conflict(subproblem.primal_solution());
    }
    return result;
}

void CutGeneration::set_lower_bound(const Var &t_var, double t_lb) {
    for (auto& subproblem : m_subproblems) {

        const bool is_applied = subproblem.set_lower_bound(t_var, t_lb);
        if(is_applied) { return; }

    }
    rmp_solution_strategy().set_lower_bound(t_var, t_lb);
}

void CutGeneration::set_upper_bound(const Var &t_var, double t_ub) {
    for (auto& subproblem : m_subproblems) {

        const bool is_applied = subproblem.set_upper_bound(t_var, t_ub);
        if (is_applied) { return; }

    }
    rmp_solution_strategy().set_upper_bound(t_var, t_ub);
}

CutGenerationSubproblem &CutGeneration::add_subproblem(const Ctr &t_cut) {
    m_subproblems.emplace_back(rmp_solution_strategy(), t_cut);
    return m_subproblems.back();
}
