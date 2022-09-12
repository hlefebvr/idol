//
// Created by henri on 12/09/22.
//
#include "solvers/solutions/Solution.h"

Solution::Solution(SolutionStatus t_status) : m_status(t_status) {

}

void Solution::set_status(SolutionStatus t_status) {
    m_status = t_status;
}

void Solution::set_value(double t_value) {
    m_objective_value = t_value;
}

void Solution::reset_objective_value() {
    m_objective_value.reset();
}

void Solution::reset_primal_values() {
    m_primal_values.reset();
}

void Solution::reset_dual_values() {
    m_dual_values.reset();
}

void Solution::reset_reduced_costs() {
    m_reduced_costs.reset();
}

void Solution::reset() {
    set_status(Unknown);
    reset_objective_value();
    reset_primal_values();
    reset_dual_values();
    reset_reduced_costs();
}

void Solution::set_primal_value(const Var &t_var, double t_value) {
    set(m_primal_values, t_var, t_value);
}

void Solution::set_reduced_cost(const Var &t_var, double t_value) {
    set(m_reduced_costs, t_var, t_value);
}

void Solution::set_dual_value(const Ctr &t_ctr, double t_value) {
    set(m_dual_values, t_ctr, t_value);
}

double Solution::value(const Var &t_var) const {
    return get(m_primal_values, t_var);
}

double Solution::reduced_cost(const Var &t_var) const {
    return get(m_reduced_costs, t_var);
}

double Solution::value(const Ctr &t_ctr) const {
    return get(m_dual_values, t_ctr);
}

SolutionStatus Solution::status() const {
    return m_status;
}

double Solution::value() const {
    if (!m_objective_value) {
        throw std::runtime_error("No solution found.");
    }
    return *m_objective_value;
}

bool Solution::has_value() const {
    return bool(m_objective_value);
}

bool Solution::has_primal_values() const {
    return bool(m_primal_values);
}

bool Solution::has_reduced_costs() const {
    return bool(m_reduced_costs);
}

bool Solution::has_dual_values() const {
    return bool(m_dual_values);
}

Solution::PrimalValues Solution::primal_values() const {
    if (!m_primal_values) {
        throw std::runtime_error("No solution found.");
    }
    return Solution::PrimalValues(*m_primal_values);
}

Solution::ReducedCosts Solution::reduced_costs() const {
    if (!m_reduced_costs) {
        throw std::runtime_error("No solution found.");
    }
    return Solution::ReducedCosts(*m_reduced_costs);
}

Solution::DualValues Solution::dual_values() const {
    if (!m_dual_values) {
        throw std::runtime_error("No solution found.");
    }
    return Solution::DualValues(*m_dual_values);
}
