//
// Created by henri on 19/09/22.
//
#include "algorithms/column-generation/ColumnGenerationSP.h"
#include "algorithms/Algorithm.h"

ColumnGenerationSP::ColumnGenerationSP(Algorithm& t_rmp_strategy)
        : m_rmp_strategy(t_rmp_strategy) {

}

void ColumnGenerationSP::solve() {
    m_exact_solution_strategy->solve();
}


Solution::Primal ColumnGenerationSP::primal_solution() const {
    const auto rmp_primals = m_rmp_strategy.primal_solution();
    return m_generator->primal_solution(*this, rmp_primals);
}


Solution::Dual ColumnGenerationSP::dual_solution() const {
    return m_exact_solution_strategy->dual_solution();
}


Row ColumnGenerationSP::get_pricing_objective(const Solution::Dual &t_duals) const {
    return m_generator->get_pricing_objective(t_duals);
}


void ColumnGenerationSP::update_pricing_objective(const Row &t_objective) {
    m_exact_solution_strategy->set_objective(t_objective);
}


void ColumnGenerationSP::build() {

    if (!m_generator) {
        throw Exception("No column generator has been given.");
    }

    if (!m_exact_solution_strategy) {
        throw Exception("No exact solution strategy has been given.");
    }

    m_exact_solution_strategy->build();
}


void ColumnGenerationSP::save_last_primal_solution() {
    m_primal_solutions.emplace_back(std::make_unique<Solution::Primal>(m_exact_solution_strategy->primal_solution()));
}


bool ColumnGenerationSP::is_unbounded() const {
    return m_primal_solutions.back()->status() == Unbounded;
}


bool ColumnGenerationSP::is_infeasible() const {
    return m_primal_solutions.back()->status() == Infeasible;
}


bool ColumnGenerationSP::could_not_be_solved_to_optimality() const {
    return m_primal_solutions.back()->status() != Optimal;
}


void ColumnGenerationSP::log_last_primal_solution() const {
    EASY_LOG(Debug, "column-generation",
             std::setw(5)
                     << "SP"
                     << std::setw(15)
                     << m_primal_solutions.back()->status()
                     << std::setw(10)
                     << m_primal_solutions.back()->objective_value()
    );
}


bool ColumnGenerationSP::improving_column_found() const {
    return m_primal_solutions.back()->objective_value() < -ToleranceForAbsoluteGapPricing;
}


void ColumnGenerationSP::add_column_to_rmp() {
    auto* last_primal_solution = m_primal_solutions.back().get();
    auto temp_var = create_column_from(*last_primal_solution);
    auto variable = m_rmp_strategy.add_column(std::move(temp_var));
    m_currently_present_variables.template emplace_back(variable, *last_primal_solution);
    EASY_LOG(Trace, "column-generation", "Adding new variable with name " << variable << ".");
}


TempVar ColumnGenerationSP::create_column_from(const Solution::Primal &t_primal_solution) const {
    return m_generator->create_column(t_primal_solution);
}

void ColumnGenerationSP::remove_column_if(const std::function<bool(const Var&, const Solution::Primal &)> &t_indicator_for_removal) {

    auto it = m_currently_present_variables.begin();
    const auto end = m_currently_present_variables.end();

    while (it != end) {
        const auto& [column_variable, ptr_to_column] = *it;
        if (t_indicator_for_removal(column_variable, ptr_to_column)) {
            m_rmp_strategy.remove_variable(column_variable);
            it = m_currently_present_variables.erase(it);
        } else {
            ++it;
        }
    }

}

bool ColumnGenerationSP::set_lower_bound(const Var &t_var, double t_lb) {
    return m_generator->set_lower_bound(t_var, t_lb, *this);
}

bool ColumnGenerationSP::set_upper_bound(const Var &t_var, double t_ub) {
    return m_generator->set_upper_bound(t_var, t_ub, *this);
}

std::optional<Ctr> ColumnGenerationSP::contribute_to_add_constraint(TempCtr &t_temporay_constraint) {
    return m_generator->contribute_to_add_constraint(t_temporay_constraint, *this);
}

bool ColumnGenerationSP::update_constraint_rhs(const Ctr &t_ctr, double t_rhs) {
    return m_generator->update_constraint_rhs(t_ctr, t_rhs, *this);
}

bool ColumnGenerationSP::remove_constraint(const Ctr &t_ctr) {
    return m_generator->remove_constraint(t_ctr, *this);
}