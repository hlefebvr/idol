//
// Created by henri on 16/09/22.
//
#include "algorithms/branch-and-cut-and-price/ColumnGenerationStrategy.h"
#include "algorithms/logs/Log.h"

ColumnGenerationStrategy::ColumnGenerationStrategy(DecompositionId&& t_id) : AbstractGenerationStrategy(std::move(t_id)) {

}

void ColumnGenerationStrategy::build() {
    for (auto& ptr_to_subproblem : m_subproblems) {
        ptr_to_subproblem->build();
    }
}

void ColumnGenerationStrategy::solve() {

    initialize();
    while (!is_terminated()) {

        rmp_solution_strategy().solve();

        save_last_rmp_dual_solution();

        log_last_rmp_dual_solution();

        analyze_last_rmp_dual_solution();

        update_subproblems();

        solve_subproblems();

        add_columns();

    }

}

Solution::Primal ColumnGenerationStrategy::primal_solution() const{
    Solution::Primal result = rmp_solution_strategy().primal_solution();
    for (auto& ptr_to_subproblem : m_subproblems) {
        result.merge_without_conflict(ptr_to_subproblem->primal_solution());
    }
    return result;
}

Solution::Dual ColumnGenerationStrategy::dual_solution() const{
    Solution::Dual result = rmp_solution_strategy().dual_solution();
    for (auto& ptr_to_subproblem : m_subproblems) {
        result.merge_without_conflict(ptr_to_subproblem->dual_solution());
    }
    return result;
}

void ColumnGenerationStrategy::set_lower_bound(const Var & t_var, double t_lb){
    for (auto& ptr_to_subproblem : m_subproblems) {
        ptr_to_subproblem->set_lower_bound(t_var, t_lb);
    }
}

void ColumnGenerationStrategy::set_upper_bound(const Var & t_var, double t_ub){
    for (auto& ptr_to_subproblem : m_subproblems) {
        ptr_to_subproblem->set_upper_bound(t_var, t_ub);
    }
}

void ColumnGenerationStrategy::initialize() {
    m_is_terminated = false;
}
void ColumnGenerationStrategy::save_last_rmp_dual_solution() {
    m_last_rmp_duals = std::make_unique<Solution::Dual>(rmp_solution_strategy().dual_solution());
}

void ColumnGenerationStrategy::analyze_last_rmp_dual_solution() {

    if (rmp_is_unbounded()) {
        terminate_for_rmp_is_unbounded();
        return;
    }

    if (rmp_is_infeasible()) {
        EASY_LOG(Trace, "column-generation", "Using Farkas certificate for pricing.");
        save_rmp_farkas();
        return;
    }

    if (rmp_could_not_be_solved_to_optimality()) {
        terminate_for_rmp_could_not_be_solved_to_optimality();
    }

    EASY_LOG(Trace, "column-generation", "Using dual solution for pricing.");

}

bool ColumnGenerationStrategy::rmp_is_unbounded() const {
    return dual(m_last_rmp_duals->status()) == Unbounded;
}

bool ColumnGenerationStrategy::rmp_is_infeasible() const {
    return dual(m_last_rmp_duals->status()) == Infeasible;
}

bool ColumnGenerationStrategy::rmp_could_not_be_solved_to_optimality() const {
    return m_last_rmp_duals->status() != Optimal;
}

void ColumnGenerationStrategy::save_rmp_farkas() {
    m_last_rmp_duals = std::make_unique<Solution::Dual>(rmp_solution_strategy().farkas_certificate());
}

void ColumnGenerationStrategy::terminate_for_rmp_could_not_be_solved_to_optimality() {
    EASY_LOG(Trace, "column-generation", "Terminate. RMP returned with dual status \"" << m_last_rmp_duals->status() << "\".");
    terminate();
}

void ColumnGenerationStrategy::terminate_for_rmp_is_unbounded() {
    EASY_LOG(Trace, "column-generation", "Terminate. Unbounded RMP.");
    terminate();
}

void ColumnGenerationStrategy::terminate() {
    m_is_terminated = true;
}

void ColumnGenerationStrategy::update_subproblems() {
    if (is_terminated()) { return; }

    for (auto& ptr_to_subproblem : m_subproblems) {
        auto row = ptr_to_subproblem->get_pricing_objective(*m_last_rmp_duals);
        EASY_LOG(Trace, "column-generation", "Setting pricing objective to " << row);
        ptr_to_subproblem->update_pricing_objective(row);
    }
}

void ColumnGenerationStrategy::solve_subproblems() {

    if (is_terminated()) { return; }

    for (auto& ptr_to_subproblem : m_subproblems) {

        ptr_to_subproblem->solve();

        ptr_to_subproblem->save_last_primal_solution();

        ptr_to_subproblem->log_last_primal_solution();

        analyze_last_subproblem_primal_solution(*ptr_to_subproblem);

        if (is_terminated()) { break; }

    }

}

void ColumnGenerationStrategy::add_columns() {

    if (is_terminated()) { return; }

    bool improving_columns_found = false;

    for (auto& ptr_to_subproblem : m_subproblems) {

        if (ptr_to_subproblem->improving_column_found()) {
            improving_columns_found = true;
            ptr_to_subproblem->add_column_to_rmp();
        }

    }

    if (!improving_columns_found) {
        terminate_for_no_improving_column_found();
    }

}

void ColumnGenerationStrategy::terminate_for_subproblem_is_infeasible() {
    EASY_LOG(Trace, "column-generation", "Terminate. Infeasible SP.");
    terminate();
}

void ColumnGenerationStrategy::terminate_for_subproblem_could_not_be_solved_to_optimality() {
    EASY_LOG(Trace, "column-generation", "Terminate. SP could not be solved to optimality using the provided exact method."
                                           "Reported status: ...............");
    terminate();
}

void ColumnGenerationStrategy::terminate_for_subproblem_is_unbounded() {
    EASY_LOG(Trace, "column-generation", "Terminate. Unbounded SP.");
    terminate();
}

void ColumnGenerationStrategy::analyze_last_subproblem_primal_solution(const AbstractColumnGenerationSubproblem &t_subproblem) {

    if (t_subproblem.is_unbounded()) {
        terminate_for_subproblem_is_unbounded();
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

void ColumnGenerationStrategy::terminate_for_no_improving_column_found() {
    EASY_LOG(Trace, "column-generation", "Terminate. No improving column found.");
    terminate();
}

void ColumnGenerationStrategy::log_last_rmp_dual_solution() const {
    EASY_LOG(Debug, "column-generation",
             std::setw(5)
             << "RMP"
             << std::setw(15)
             << dual(m_last_rmp_duals->status())
             << std::setw(10)
             << m_last_rmp_duals->objective_value()
    );
}
