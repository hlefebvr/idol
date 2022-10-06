//
// Created by henri on 05/10/22.
//
#include <iomanip>
#include "algorithms/solution-strategies/cut-generation/subproblems/CutGenerationSubproblem.h"
#include "algorithms/logs/Log.h"

CutGenerationSubproblem::CutGenerationSubproblem(Algorithm &t_rmp_strategy) : m_rmp_strategy(t_rmp_strategy) {

}

void CutGenerationSubproblem::build() {

    if (!m_generator) {
        throw Exception("No column generator has been given.");
    }

    if (!m_exact_solution_strategy) {
        throw Exception("No exact solution strategy has been given.");
    }

    m_exact_solution_strategy->build();
}

void CutGenerationSubproblem::solve() {
    m_exact_solution_strategy->solve();
}

Row CutGenerationSubproblem::get_separation_objective(const Solution::Primal &t_primals) const {
    return m_generator->get_separation_objective(t_primals);
}

void CutGenerationSubproblem::update_separation_objective(const Row &t_objective) {
    m_exact_solution_strategy->set_objective(t_objective);
}

void CutGenerationSubproblem::save_last_primal_solution() {
    m_primal_solutions.emplace_back(std::make_unique<Solution::Primal>(m_exact_solution_strategy->primal_solution()));
}

void CutGenerationSubproblem::log_last_primal_solution() {
    EASY_LOG(Debug, "cut-generation",
             std::setw(5)
             << "SP"
             << std::setw(15)
             << m_primal_solutions.back()->status()
             << std::setw(10)
             << m_primal_solutions.back()->objective_value()
    );
}

bool CutGenerationSubproblem::violated_cut_found() {
    return m_primal_solutions.back()->objective_value() < -ToleranceForAbsoluteGapPricing;
}

void CutGenerationSubproblem::add_cut_to_rmp() {
    auto* last_primal_solution = m_primal_solutions.back().get();
    auto temp_ctr = create_cut_from(*last_primal_solution);
    auto constraint = m_rmp_strategy.add_constraint(std::move(temp_ctr));
    m_currently_present_cuts.template emplace_back(constraint, *last_primal_solution);
    EASY_LOG(Trace, "cut-generation", "Adding new constraint " << constraint << ".");
}

bool CutGenerationSubproblem::is_unbounded() const {
    return m_primal_solutions.back()->status() == Unbounded;
}

bool CutGenerationSubproblem::is_infeasible() const {
    return m_primal_solutions.back()->status() == Infeasible;
}

bool CutGenerationSubproblem::could_not_be_solved_to_optimality() const {
    return m_primal_solutions.back()->status() != Optimal;
}

TempCtr CutGenerationSubproblem::create_cut_from(const Solution::Primal &t_primals) const {
    return m_generator->create_cut(t_primals);
}

Solution::Primal CutGenerationSubproblem::primal_solution() const {
    const auto rmp_duals = m_rmp_strategy.dual_solution();
    return m_generator->primal_solution(*this, rmp_duals);
}

bool CutGenerationSubproblem::set_lower_bound(const Var &t_var, double t_lb) {
    return m_generator->set_lower_bound(t_var, t_lb, *this);
}

bool CutGenerationSubproblem::set_upper_bound(const Var &t_var, double t_ub) {
    return m_generator->set_upper_bound(t_var, t_ub, *this);
}

void CutGenerationSubproblem::remove_cut_if(const std::function<bool(const Ctr &, const Solution::Primal &)> &t_indicator_for_removal) {

    auto it = m_currently_present_cuts.begin();
    const auto end = m_currently_present_cuts.end();

    while (it != end) {
        const auto& [column_variable, ptr_to_column] = *it;
        if (t_indicator_for_removal(column_variable, ptr_to_column)) {
            m_rmp_strategy.remove_constraint(column_variable);
            it = m_currently_present_cuts.erase(it);
        } else {
            ++it;
        }
    }

}
