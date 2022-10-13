//
// Created by henri on 05/10/22.
//
#include <iomanip>
#include "../../../include/algorithms/row-generation/RowGenerationSP.h"
#include "../../../include/algorithms/logs/Log.h"

RowGenerationSP::RowGenerationSP(Algorithm &t_rmp_strategy, const Ctr& t_cut)
    : m_rmp_strategy(t_rmp_strategy),
      m_cut_template(Row(t_cut.row()), t_cut.type()),
      m_objective_template(t_cut.row().transpose()) {

    save_subproblem_ids();
    remove_cut_template_from_rmp(t_cut);

}

void RowGenerationSP::save_subproblem_ids() {
    for (const auto& [var, constant] : m_objective_template.lhs()) {
        m_subproblem_ids.emplace(var.model_id());
    }
}

void RowGenerationSP::remove_cut_template_from_rmp(const Ctr& t_cut) {
    EASY_LOG(Trace, "row-generation", "Constraint " << t_cut << " has been removed from the RMP for it will be generated.");
    rmp_solution_strategy().remove(t_cut);
}

void RowGenerationSP::initialize() {

    if (!m_exact_solution_strategy) {
        throw Exception("No exact solution strategy has been given.");
    }

}

void RowGenerationSP::solve() {
    m_exact_solution_strategy->solve();
}

Row RowGenerationSP::get_separation_objective(const Solution::Primal &t_primals) const {
    double sign = m_cut_template.type() == LessOrEqual ? 1. : -1.;

    Row result;

    for (const auto& [var, constant] : m_objective_template.lhs()) {
        result.lhs() += sign * constant.fix(t_primals) * var;
    }

    result.rhs() = sign * m_objective_template.rhs().fix(t_primals);

    return result;
}

void RowGenerationSP::update_separation_objective(const Row &t_objective) {
    m_exact_solution_strategy->update_objective(t_objective);
}

void RowGenerationSP::save_last_primal_solution() {
    m_primal_solutions.emplace_back(std::make_unique<Solution::Primal>(m_exact_solution_strategy->primal_solution()));
}

void RowGenerationSP::log_last_primal_solution() {
    EASY_LOG(Debug, "row-generation",
             std::setw(5)
             << "SP"
             << std::setw(15)
             << m_primal_solutions.back()->status()
             << std::setw(10)
             << m_primal_solutions.back()->objective_value()
    );
}

bool RowGenerationSP::violated_cut_found() {
    return m_primal_solutions.back()->objective_value() < -ToleranceForAbsoluteGapPricing;
}

void RowGenerationSP::add_cut_to_rmp() {
    auto* last_primal_solution = m_primal_solutions.back().get();
    auto temp_ctr = create_cut_from(*last_primal_solution);
    auto constraint = m_rmp_strategy.add_row(std::move(temp_ctr));
    m_currently_present_cuts.template emplace_back(constraint, *last_primal_solution);
    //TODO allow logs: EASY_LOG(Trace, "row-generation", "Adding new constraint " << constraint << ".");
}

bool RowGenerationSP::is_unbounded() const {
    return m_primal_solutions.back()->status() == Unbounded;
}

bool RowGenerationSP::is_infeasible() const {
    return m_primal_solutions.back()->status() == Infeasible;
}

bool RowGenerationSP::could_not_be_solved_to_optimality() const {
    return m_primal_solutions.back()->status() != Optimal;
}

TempCtr RowGenerationSP::create_cut_from(const Solution::Primal &t_primals) const {
    return { m_cut_template.row().fix(t_primals), m_cut_template.type() };
}

Solution::Primal RowGenerationSP::primal_solution() const {

    if (!m_original_space_builder) {
        return {};
    }

    return m_original_space_builder->primal_solution(*this, m_rmp_strategy);
}

bool RowGenerationSP::set_lower_bound(const Var &t_var, double t_lb) {
    if (!is_in_subproblem(t_var)) { return false; }

    remove_cuts_violating_lower_bound(t_var, t_lb);
    exact_solution_strategy().update_lb(t_var, t_lb);

    return true;
}

bool RowGenerationSP::set_upper_bound(const Var &t_var, double t_ub) {
    if (!is_in_subproblem(t_var)) { return false; }

    remove_cuts_violating_upper_bound(t_var, t_ub);
    exact_solution_strategy().update_ub(t_var, t_ub);

    return true;
}

void RowGenerationSP::remove_cut_if(const std::function<bool(const Ctr &, const Solution::Primal &)> &t_indicator_for_removal) {

    auto it = m_currently_present_cuts.begin();
    const auto end = m_currently_present_cuts.end();

    while (it != end) {
        const auto& [column_variable, ptr_to_column] = *it;
        if (t_indicator_for_removal(column_variable, ptr_to_column)) {
            m_rmp_strategy.remove(column_variable);
            it = m_currently_present_cuts.erase(it);
        } else {
            ++it;
        }
    }

}

bool RowGenerationSP::is_in_subproblem(const Var &t_var) const {
    return m_subproblem_ids.find(t_var.id()) != m_subproblem_ids.end();
}

void RowGenerationSP::remove_cuts_violating_lower_bound(const Var &t_var, double t_lb) {

    remove_cut_if([&](const Ctr& t_cut, const auto& t_cut_primal_solution){
        if (double value = t_cut_primal_solution.get(t_var) ; value < t_lb + ToleranceForIntegrality) {
            EASY_LOG(Trace,
                     "row-generation",
                     "Cut " << t_cut << " was removed by contradiction with required "
                            << "bound " << t_var << " >= " << t_lb << " (" << t_var << " = " << value << ").");
            return true;
        }
        return false;
    });

}

void RowGenerationSP::remove_cuts_violating_upper_bound(const Var &t_var, double t_ub) {

    remove_cut_if([&](const Ctr& t_cut, const auto& t_cut_primal_solution){
        if (double value = t_cut_primal_solution.get(t_var) ; value > t_ub - ToleranceForIntegrality) {
            EASY_LOG(Trace,
                     "row-generation",
                     "Cut " << t_cut << " was removed by contradiction with required "
                            << "bound " << t_var << " <= " << t_ub << " (" << t_var << " = " << value << ").");
            return true;
        }
        return false;
    });

}
