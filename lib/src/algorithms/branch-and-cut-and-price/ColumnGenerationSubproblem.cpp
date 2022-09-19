//
// Created by henri on 19/09/22.
//
#include "algorithms/branch-and-cut-and-price/ColumnGenerationSubproblem.h"

ColumnGenerationSubProblem::ColumnGenerationSubProblem(AbstractSolutionStrategy& t_rmp_strategy, ColumnGenerator t_generator, AbstractSolutionStrategy* t_exact_solution_strategy)
        : m_generator(std::move(t_generator)),
          m_rmp_strategy(t_rmp_strategy),
          m_exact_solution_strategy(t_exact_solution_strategy) {

}

void ColumnGenerationSubProblem::solve() {
    m_exact_solution_strategy->solve();
}


Solution::Primal ColumnGenerationSubProblem::primal_solution() const {
    const auto rmp_primals = m_rmp_strategy.primal_solution();
    Solution::Primal result;
    for (const auto& [var, ptr_to_sol] : m_currently_present_variables) {
        result += rmp_primals.get(var) * (*ptr_to_sol);
    }
    return result;
}


Solution::Dual ColumnGenerationSubProblem::dual_solution() const {
    return m_exact_solution_strategy->dual_solution();
}


Row ColumnGenerationSubProblem::get_pricing_objective(const Solution::Dual &t_duals) const {
    // TODO what about objective ?
    Row result;
    for (auto [ctr, row] : m_generator) {
        row *= -t_duals.get(ctr);
        result += row;
    }
    return result;
}


void ColumnGenerationSubProblem::update_pricing_objective(const Row &t_objective) {
    m_exact_solution_strategy->set_objective(t_objective);
}


void ColumnGenerationSubProblem::build() {
    m_exact_solution_strategy->build();
}


void ColumnGenerationSubProblem::save_last_primal_solution() {
    m_primal_solutions.emplace_back(std::make_unique<Solution::Primal>(m_exact_solution_strategy->primal_solution()));
}


bool ColumnGenerationSubProblem::is_unbounded() const {
    return m_primal_solutions.back()->status() == Unbounded;
}


bool ColumnGenerationSubProblem::is_infeasible() const {
    return m_primal_solutions.back()->status() == Infeasible;
}


bool ColumnGenerationSubProblem::could_not_be_solved_to_optimality() const {
    return m_primal_solutions.back()->status() != Optimal;
}


void ColumnGenerationSubProblem::log_last_primal_solution() const {
    EASY_LOG(Debug, "column-generation",
             std::setw(5)
                     << "SP"
                     << std::setw(15)
                     << m_primal_solutions.back()->status()
                     << std::setw(10)
                     << m_primal_solutions.back()->objective_value()
    );
}


bool ColumnGenerationSubProblem::improving_column_found() const {
    return m_primal_solutions.back()->objective_value() < -ToleranceForAbsoluteGapPricing;
}


void ColumnGenerationSubProblem::add_column_to_rmp() {
    auto* last_primal_solution = m_primal_solutions.back().get();
    auto temp_var = create_column_from(*last_primal_solution);
    auto variable = m_rmp_strategy.add_column(std::move(temp_var));
    m_currently_present_variables.template emplace(variable, last_primal_solution);
    EASY_LOG(Trace, "column-generation", "Adding new variable with name " << variable << ".");
}


TempVar ColumnGenerationSubProblem::create_column_from(const Solution::Primal &t_primal_solution) const {

    Column column;

    //column.set_constant(m_last_primal_solution->objective_value());
    // evaluate value from original objective

    for (const auto& [ctr, expr] : m_generator) {
        double coeff = expr.constant().constant();
        for (const auto& [var, value] : expr) {
            coeff += value.constant() * t_primal_solution.get(var);
        }
        column.set(ctr, coeff);
    }

    return { m_generator.lb(), m_generator.ub(), m_generator.type(), std::move(column) };
}


void ColumnGenerationSubProblem::set_lower_bound(const Var &t_var, double t_lb) {
    for (const auto& [var, ptr_to_col] : m_currently_present_variables) {
        m_rmp_strategy.remove_variable(var);
    }
    m_currently_present_variables.clear();

    m_exact_solution_strategy->set_lower_bound(t_var, t_lb);
}


void ColumnGenerationSubProblem::set_upper_bound(const Var &t_var, double t_ub) {
    for (const auto& [var, ptr_to_col] : m_currently_present_variables) {
        m_rmp_strategy.remove_variable(var);
    }
    m_currently_present_variables.clear();

    m_exact_solution_strategy->set_upper_bound(t_var, t_ub);
}
