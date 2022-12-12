//
// Created by henri on 05/10/22.
//
#include <iomanip>
#include "../../../include/algorithms/row-generation/RowGeneration.h"
#include "../../../include/modeling/expressions/Expr.h"

RowGeneration::RowGeneration(Algorithm &t_rmp_solution_strategy) : GenerationAlgorithm(t_rmp_solution_strategy){

}

void RowGeneration::execute() {

    initialize();

    while (!is_terminated()) {

        solve_rmp();

        save_last_rmp_primal_solution();

        log_last_rmp_primal_solution();

        analyze_last_rmp_primal_solution();

        update_subproblems();

        solve_subproblems();

        add_cuts();

        ++m_iteration;

        if (iteration_limit_is_reached()) {
            terminate_for_iteration_limit_is_reached();
        }

        if (time_limit_is_reached() ) {
            terminate_for_time_limit_is_reached();
        }

    }

}

void RowGeneration::initialize() {
    m_is_terminated = false;
    m_iteration = 0;
    m_violated_cut_found_at_last_iteration = true;

    for (auto& subproblem : m_subproblems) {
        subproblem.initialize();
    }
}

void RowGeneration::solve_rmp() {
    rmp_solution_strategy().set(Param::Algorithm::TimeLimit, remaining_time());
    rmp_solution_strategy().solve();
}

void RowGeneration::save_last_rmp_primal_solution() {
    m_last_rmp_primals = std::make_unique<Solution::Primal>(rmp_solution_strategy().primal_solution());
}

void RowGeneration::analyze_last_rmp_primal_solution() {

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

    idol_Log(Trace, "row-generation", "Using primal solution for separation.");

}

bool RowGeneration::rmp_is_unbounded() const {
    return m_last_rmp_primals->status() == Unbounded;
}

bool RowGeneration::rmp_is_infeasible() const {
    return m_last_rmp_primals->status() == Infeasible;
}

bool RowGeneration::rmp_could_not_be_solved_to_optimality() const {
    return m_last_rmp_primals->status() != Optimal;
}

void RowGeneration::terminate_for_rmp_is_infeasible() {
    idol_Log(Trace, "row-generation", "Terminate. Infeasible RMP.");
    terminate();
}

void RowGeneration::terminate_for_rmp_could_not_be_solved_to_optimality() {
    idol_Log(Trace, "row-generation", "Terminate. RMP returned with primal status \"" << m_last_rmp_primals->status() << "\".");
    terminate();
}

void RowGeneration::terminate() {
    m_is_terminated = true;
}

void RowGeneration::update_subproblems() {
    if (is_terminated()) { return; }

    for (auto& subproblem : m_subproblems) {
        auto row = subproblem.get_separation_objective(*m_last_rmp_primals);
        idol_Log(Trace, "row-generation", "Setting separation objective to " << row);
        subproblem.update_separation_objective(row);
    }
}

void RowGeneration::solve_subproblems() {

    if (is_terminated()) { return; }

    for (auto& subproblem : m_subproblems) {

        subproblem.solve();

        subproblem.save_last_primal_solution();

        subproblem.log_last_primal_solution();

        analyze_last_subproblem_primal_solution(subproblem);

        if (is_terminated()) { break; }

    }

}

void RowGeneration::add_cuts() {

    if (is_terminated()) { return; }

    m_violated_cut_found_at_last_iteration = false;

    for (auto& subproblem : m_subproblems) {

        if (subproblem.violated_cut_found()) {
            m_violated_cut_found_at_last_iteration = true;
            subproblem.add_cut_to_rmp();
        }

    }

    if (!m_violated_cut_found_at_last_iteration) {
        terminate_for_no_violated_cut_found();
    }

}

void RowGeneration::terminate_for_no_violated_cut_found() {
    idol_Log(Trace, "row-generation", "Terminate. No violated cut found.");
    terminate();
}

void
RowGeneration::analyze_last_subproblem_primal_solution(const RowGenerationSP &t_subproblem) {

    if (t_subproblem.is_unbounded()) {
        throw Exception("Did not know what to do with unbounded SP.");
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

void RowGeneration::terminate_for_subproblem_is_infeasible() {
    idol_Log(Trace, "row-generation", "Terminate. Infeasible SP.");
    terminate();
}

void RowGeneration::terminate_for_subproblem_could_not_be_solved_to_optimality() {
    idol_Log(Trace, "row-generation", "Terminate. SP could not be solved to optimality using the provided exact method."
                                         "Reported status: ...............");
    terminate();
}

void RowGeneration::log_last_rmp_primal_solution() const {
    idol_Log(Debug, "row-generation",
             std::setw(5)
             << "RMP"
             << std::setw(15)
             << m_last_rmp_primals->status()
             << std::setw(15)
             << m_last_rmp_primals->reason()
             << std::setw(15)
             << m_last_rmp_primals->objective_value()
    );
}

Solution::Primal RowGeneration::primal_solution() const {
    Solution::Primal result = rmp_solution_strategy().primal_solution();
    for (auto& subproblem : m_subproblems) {
        result.merge_without_conflict(subproblem.primal_solution());
    }
    return result;
}

RowGenerationSP &RowGeneration::add_subproblem(const Ctr &t_cut) {
    m_subproblems.emplace_back(rmp_solution_strategy(), t_cut);
    return m_subproblems.back();
}

bool RowGeneration::iteration_limit_is_reached() const {
    return m_iteration >= get(Param::Algorithm::MaxIterations);
}

void RowGeneration::terminate_for_iteration_limit_is_reached() {
    idol_Log(Trace, "row-generation", "Terminate. The maximum number of iterations has been reached.")
    terminate();
}

bool RowGeneration::time_limit_is_reached() const {
    return remaining_time() <= 0.;
}

void RowGeneration::terminate_for_time_limit_is_reached() {
    if (is_terminated()) { return; }
    idol_Log(Trace, "row-generation", "Terminate. The time limit has been reached.");
    std::cout << "TIME_LIMIT" << std::endl;
    terminate();
}
