//
// Created by henri on 16/09/22.
//
#include "algorithms/column-generation/ColumnGeneration.h"
#include "algorithms/logs/Log.h"

ColumnGeneration::ColumnGeneration(DecompositionId&& t_id) : GenerationAlgorithm(std::move(t_id)) {

}

void ColumnGeneration::build() {
    for (auto& subproblem : m_subproblems) {
        subproblem.build();
    }
}

void ColumnGeneration::solve() {

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

Solution::Primal ColumnGeneration::primal_solution() const{
    Solution::Primal result = rmp_solution_strategy().primal_solution();
    for (auto& subproblem : m_subproblems) {
        result.merge_without_conflict(subproblem.primal_solution());
    }
    return result;
}

Solution::Dual ColumnGeneration::dual_solution() const{
    Solution::Dual result = rmp_solution_strategy().dual_solution();
    for (auto& subproblem : m_subproblems) {
        result.merge_without_conflict(subproblem.dual_solution());
    }
    return result;
}

void ColumnGeneration::set_lower_bound(const Var & t_var, double t_lb){
    for (auto& subproblem : m_subproblems) {

        const bool is_applied = subproblem.set_lower_bound(t_var, t_lb);
        if(is_applied) { return; }

    }
    rmp_solution_strategy().set_lower_bound(t_var, t_lb);
}

void ColumnGeneration::set_upper_bound(const Var & t_var, double t_ub){
    for (auto& subproblem : m_subproblems) {

        const bool is_applied = subproblem.set_upper_bound(t_var, t_ub);
        if (is_applied) { return; }

    }
    rmp_solution_strategy().set_upper_bound(t_var, t_ub);
}

void ColumnGeneration::initialize() {
    m_is_terminated = false;
}
void ColumnGeneration::save_last_rmp_dual_solution() {
    m_last_rmp_duals = std::make_unique<Solution::Dual>(rmp_solution_strategy().dual_solution());
}

void ColumnGeneration::analyze_last_rmp_dual_solution() {

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

bool ColumnGeneration::rmp_is_unbounded() const {
    return dual(m_last_rmp_duals->status()) == Unbounded;
}

bool ColumnGeneration::rmp_is_infeasible() const {
    return dual(m_last_rmp_duals->status()) == Infeasible;
}

bool ColumnGeneration::rmp_could_not_be_solved_to_optimality() const {
    return m_last_rmp_duals->status() != Optimal;
}

void ColumnGeneration::save_rmp_farkas() {
    m_last_rmp_duals = std::make_unique<Solution::Dual>(rmp_solution_strategy().farkas_certificate());
}

void ColumnGeneration::terminate_for_rmp_could_not_be_solved_to_optimality() {
    EASY_LOG(Trace, "column-generation", "Terminate. RMP returned with dual status \"" << m_last_rmp_duals->status() << "\".");
    terminate();
}

void ColumnGeneration::terminate_for_rmp_is_unbounded() {
    EASY_LOG(Trace, "column-generation", "Terminate. Unbounded RMP.");
    terminate();
}

void ColumnGeneration::terminate() {
    m_is_terminated = true;
}

void ColumnGeneration::update_subproblems() {
    if (is_terminated()) { return; }

    for (auto& subproblem : m_subproblems) {
        auto row = subproblem.get_pricing_objective(*m_last_rmp_duals);
        EASY_LOG(Trace, "column-generation", "Setting pricing objective to " << row);
        subproblem.update_pricing_objective(row);
    }
}

void ColumnGeneration::solve_subproblems() {

    if (is_terminated()) { return; }

    for (auto& subproblem : m_subproblems) {

        subproblem.solve();

        subproblem.save_last_primal_solution();

        subproblem.log_last_primal_solution();

        analyze_last_subproblem_primal_solution(subproblem);

        if (is_terminated()) { break; }

    }

}

void ColumnGeneration::add_columns() {

    if (is_terminated()) { return; }

    bool improving_columns_found = false;

    for (auto& subproblem : m_subproblems) {

        if (subproblem.improving_column_found()) {
            improving_columns_found = true;
            subproblem.add_column_to_rmp();
        }

    }

    if (!improving_columns_found) {
        terminate_for_no_improving_column_found();
    }

}

void ColumnGeneration::terminate_for_subproblem_is_infeasible() {
    EASY_LOG(Trace, "column-generation", "Terminate. Infeasible SP.");
    terminate();
}

void ColumnGeneration::terminate_for_subproblem_could_not_be_solved_to_optimality() {
    EASY_LOG(Trace, "column-generation", "Terminate. SP could not be solved to optimality using the provided exact method."
                                           "Reported status: ...............");
    terminate();
}

void ColumnGeneration::terminate_for_subproblem_is_unbounded() {
    EASY_LOG(Trace, "column-generation", "Terminate. Unbounded SP.");
    terminate();
}

void ColumnGeneration::analyze_last_subproblem_primal_solution(const ColumnGenerationSP &t_subproblem) {

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

void ColumnGeneration::terminate_for_no_improving_column_found() {
    EASY_LOG(Trace, "column-generation", "Terminate. No improving column found.");
    terminate();
}

void ColumnGeneration::log_last_rmp_dual_solution() const {
    EASY_LOG(Debug, "column-generation",
             std::setw(5)
             << "RMP"
             << std::setw(15)
             << dual(m_last_rmp_duals->status())
             << std::setw(10)
             << m_last_rmp_duals->objective_value()
    );
}

Ctr ColumnGeneration::add_constraint(TempCtr t_temporary_constraint) {

    for (auto& subproblem : m_subproblems) {

        auto optional_ctr = subproblem.add_constraint(t_temporary_constraint);

        if (optional_ctr.has_value()) {
            return optional_ctr.value();
        }

    }

    return rmp_solution_strategy().add_constraint(std::move(t_temporary_constraint));

}

void ColumnGeneration::update_constraint_rhs(const Ctr &t_ctr, double t_rhs) {

    for (auto& subproblem : m_subproblems) {

        if (subproblem.update_constraint_rhs(t_ctr, t_rhs)) {
            return;
        }

    }

}

void ColumnGeneration::remove_constraint(const Ctr &t_constraint) {

    for (auto& subproblem : m_subproblems) {

        if (subproblem.remove_constraint(t_constraint)) {
            return;
        }

    }

}

ColumnGenerationSP &ColumnGeneration::add_subproblem(const Var& t_var) {
    m_subproblems.emplace_back(rmp_solution_strategy(), t_var);
    return m_subproblems.back();
}
