//
// Created by henri on 16/09/22.
//
#include <omp.h>
#include "../../../include/algorithms/column-generation/ColumnGeneration.h"
#include "../../../include/backends/parameters/Logs.h"
#include "backends/column-generation/ColumnGeneration.h"


ColumnGeneration::ColumnGeneration(Algorithm& t_rmp_solution_strategy) : GenerationAlgorithm(t_rmp_solution_strategy) {

}

void ColumnGeneration::execute() {

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
        //for (const auto& [var, column] : subproblem.currently_present_variables()) {
        //    result.set(var, 0.);
        //}
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

void ColumnGeneration::initialize() {
    for (auto& subproblem : m_subproblems) {
        subproblem.initialize();
    }
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
        idol_Log(Trace, ColumnGeneration, "Using Farkas certificate for pricing.");
        save_rmp_farkas();
        return;
    }

    if (rmp_could_not_be_solved_to_optimality()) {
        terminate_for_rmp_could_not_be_solved_to_optimality();
    }

    idol_Log(Trace, ColumnGeneration, "Using dual solution for pricing.");

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
    idol_Log(Trace, ColumnGeneration, "Terminate. RMP returned with dual status \"" << m_last_rmp_duals->status() << "\".");
    terminate();
}

void ColumnGeneration::terminate_for_rmp_is_unbounded() {
    idol_Log(Trace, ColumnGeneration, "Terminate. Unbounded RMP.");
    terminate();
}

void ColumnGeneration::update_subproblems() {
    if (is_terminated()) { return; }

    for (auto& subproblem : m_subproblems) {
        auto row = subproblem.get_pricing_objective(*m_last_rmp_duals);
        idol_Log(Trace, ColumnGeneration, "Setting pricing objective to " << row);
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
    idol_Log(Trace, ColumnGeneration, "Terminate. Infeasible SP.");
    terminate();
}

void ColumnGeneration::terminate_for_subproblem_could_not_be_solved_to_optimality() {
    idol_Log(Trace, ColumnGeneration, "Terminate. SP could not be solved to optimality using the provided exact method."
                                           "Reported status: ...............");
    terminate();
}

void ColumnGeneration::terminate_for_subproblem_is_unbounded() {
    idol_Log(Trace, ColumnGeneration, "Terminate. Unbounded SP.");
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
    idol_Log(Trace, ColumnGeneration, "Terminate. No improving column found.");
    terminate();
}

void ColumnGeneration::log_last_rmp_dual_solution() const {
    idol_Log(Debug, ColumnGeneration,
             std::setw(5)
             << "RMP"
             << std::setw(15)
             << dual(m_last_rmp_duals->status())
             << std::setw(10)
             << m_last_rmp_duals->objective_value()
    );
}

Ctr ColumnGeneration::add_ctr(TempCtr&& t_temporary_constraint) {

    for (auto& subproblem : m_subproblems) {

        auto optional_ctr = subproblem.add_constraint(t_temporary_constraint);

        if (optional_ctr.has_value()) {
            return optional_ctr.value();
        }

    }

    throw std::runtime_error("Not implemented!");

}

void ColumnGeneration::remove(const Ctr &t_constraint) {

    for (auto& subproblem : m_subproblems) {

        if (subproblem.remove_constraint(t_constraint)) {
            return;
        }

    }

    idol_Log(Trace, ColumnGeneration, "Constraint " << t_constraint << " has been removed from RMP.");
    rmp_solution_strategy().remove(t_constraint);

}

ColumnGenerationSP &ColumnGeneration::add_subproblem(const Var& t_var) {
    m_subproblems.emplace_back(rmp_solution_strategy(), t_var);
    return m_subproblems.back();
}

AttributeManager &ColumnGeneration::attribute_delegate(const Attribute &t_attribute) {
    return rmp_solution_strategy();
}

AttributeManager &ColumnGeneration::attribute_delegate(const Attribute &t_attribute, const Var &t_object) {
    if (rmp_solution_strategy().get(Attr::Var::Status, t_object)) {
        return rmp_solution_strategy();
    }
    for (auto& subproblem : m_subproblems) {
        if (subproblem.exact_solution_strategy().get(Attr::Var::Status, t_object)) {
            return subproblem.exact_solution_strategy();
        }
    }
    return Delegate::attribute_delegate(t_attribute, t_object);
}

AttributeManager &ColumnGeneration::attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) {
    if (rmp_solution_strategy().get(Attr::Ctr::Status, t_object)) {
        return rmp_solution_strategy();
    }
    for (auto& subproblem : m_subproblems) {
        if (subproblem.exact_solution_strategy().get(Attr::Ctr::Status, t_object)) {
            return subproblem.exact_solution_strategy();
        }
    }
    return Delegate::attribute_delegate(t_attribute, t_object);
}

void ColumnGeneration::set(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) {

    if (rmp_solution_strategy().get(Attr::Var::Status, t_var)) {
        return rmp_solution_strategy().set(t_attr, t_var, t_value);
    }

    ColumnGenerationSP* sp = nullptr;

    for (auto& subproblem : m_subproblems) {
        if (subproblem.exact_solution_strategy().get(Attr::Var::Status, t_var)) {
            sp = &subproblem;
            break;
        }
    }

    if (sp == nullptr) {
        Delegate::set(t_attr, t_var, t_value);
        return;
    }

    if (t_attr == Attr::Var::Lb) {
        sp->set_lower_bound(t_var, t_value);
        return;
    }

    if (t_attr == Attr::Var::Ub) {
        sp->set_upper_bound(t_var, t_value);
        return;
    }

    Delegate::set(t_attr, t_var, t_value);
}
