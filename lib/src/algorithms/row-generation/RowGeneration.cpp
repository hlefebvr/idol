//
// Created by henri on 05/10/22.
//
#include <iomanip>
#include "../../../include/algorithms/row-generation/RowGeneration.h"
#include "../../../include/modeling/expressions/Expr.h"

RowGeneration::RowGeneration(Algorithm &t_rmp_solution_strategy) : GenerationAlgorithmWithAttributes(t_rmp_solution_strategy){

    /*
    set_callback_attribute<Attr::CutOff>([this](double t_cutoff){
        rmp_solution_strategy().set<Attr::CutOff>(t_cutoff);
    });
    */

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

    if (get<Attr::SubOptimalRMP>()) {
        m_rmp_solved_to_optimality = false;
        rmp_solution_strategy().set<Attr::MipGap>(get<Attr::SubOptimalRMP_Tolerance>());
    }

    for (auto& subproblem : m_subproblems) {
        subproblem.initialize();
    }
}

void RowGeneration::solve_rmp() {
    const double remaining_time = std::max(0., get<Attr::TimeLimit>() - time().count());
    rmp_solution_strategy().set<Attr::TimeLimit>(remaining_time);
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
        if (!m_rmp_solved_to_optimality) {
            rmp_solution_strategy().set<Attr::MipGap>(get<Attr::MipGap>());
            m_rmp_solved_to_optimality = true;
        } else {
            terminate_for_no_violated_cut_found();
        }
    } else if (m_rmp_solved_to_optimality && get<Attr::SubOptimalRMP>()) {
        //rmp_solution_strategy().set<Attr::MipGap>(get<Attr::SubOptimalRMP_Tolerance>());
        //m_rmp_solved_to_optimality = false;
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

void RowGeneration::update_var_lb(const Var &t_var, double t_lb) {
    for (auto& subproblem : m_subproblems) {

        const bool is_applied = subproblem.set_lower_bound(t_var, t_lb);
        if(is_applied) { return; }

    }
    rmp_solution_strategy().update_var_lb(t_var, t_lb);
}

void RowGeneration::update_var_ub(const Var &t_var, double t_ub) {
    for (auto& subproblem : m_subproblems) {

        const bool is_applied = subproblem.set_upper_bound(t_var, t_ub);
        if (is_applied) { return; }

    }
    rmp_solution_strategy().update_var_ub(t_var, t_ub);
}

RowGenerationSP &RowGeneration::add_subproblem(const Ctr &t_cut) {
    m_subproblems.emplace_back(rmp_solution_strategy(), t_cut);
    return m_subproblems.back();
}

bool RowGeneration::iteration_limit_is_reached() const {
    return m_iteration >= get<Attr::MaxIterations>();
}

void RowGeneration::terminate_for_iteration_limit_is_reached() {
    idol_Log(Trace, "row-generation", "Terminate. The maximum number of iterations has been reached.")
    terminate();
}

bool RowGeneration::time_limit_is_reached() const {
    return get<Attr::TimeLimit>() <= time().count();
}

void RowGeneration::terminate_for_time_limit_is_reached() {
    if (is_terminated()) { return; }
    idol_Log(Trace, "row-generation", "Terminate. The time limit has been reached.");
    std::cout << "TIME_LIMIT" << std::endl;
    terminate();
}

double RowGeneration::get_lb(const Var &t_var) const {
    for (auto& subproblem : m_subproblems) {
        if (subproblem.exact_solution_strategy().has(t_var)) {
            return subproblem.exact_solution_strategy().get_lb(t_var);
        }
    }
    return rmp_solution_strategy().get_lb(t_var);
}

double RowGeneration::get_ub(const Var &t_var) const {
    for (auto& subproblem : m_subproblems) {
        if (subproblem.exact_solution_strategy().has(t_var)) {
            return subproblem.exact_solution_strategy().get_ub(t_var);
        }
    }
    return rmp_solution_strategy().get_ub(t_var);
}

VarType RowGeneration::get_type(const Var &t_var) const {
    for (auto& subproblem : m_subproblems) {
        if (subproblem.exact_solution_strategy().has(t_var)) {
            return subproblem.exact_solution_strategy().get_type(t_var);
        }
    }
    return rmp_solution_strategy().get_type(t_var);
}

const Column &RowGeneration::get_column(const Var &t_var) const {
    for (auto& subproblem : m_subproblems) {
        if (subproblem.exact_solution_strategy().has(t_var)) {
            return subproblem.exact_solution_strategy().get_column(t_var);
        }
    }
    return rmp_solution_strategy().get_column(t_var);
}

bool RowGeneration::has(const Var &t_var) const {
    for (auto& subproblem : m_subproblems) {
        if (subproblem.exact_solution_strategy().has(t_var)) {
            return true;
        }
    }
    return rmp_solution_strategy().has(t_var);
}

bool RowGeneration::has(const Ctr &t_ctr) const {
    for (auto& subproblem : m_subproblems) {
        if (subproblem.exact_solution_strategy().has(t_ctr)) {
            return true;
        }
    }
    return rmp_solution_strategy().has(t_ctr);
}

const Row &RowGeneration::get_row(const Ctr &t_ctr) const {
    for (auto& subproblem : m_subproblems) {
        if (subproblem.exact_solution_strategy().has(t_ctr)) {
            return subproblem.exact_solution_strategy().get_row(t_ctr);
        }
    }
    return rmp_solution_strategy().get_row(t_ctr);
}

CtrType RowGeneration::get_type(const Ctr &t_ctr) const {
    for (auto& subproblem : m_subproblems) {
        if (subproblem.exact_solution_strategy().has(t_ctr)) {
            return subproblem.exact_solution_strategy().get_type(t_ctr);
        }
    }
    return rmp_solution_strategy().get_type(t_ctr);
}

