//
// Created by henri on 05/10/22.
//
#include <iomanip>
#include "../../../include/algorithms/cut-generation/CutGenerationSP.h"
#include "../../../include/algorithms/logs/Log.h"

CutGenerationSP::CutGenerationSP(Algorithm &t_rmp_strategy, const Ctr& t_cut)
    : m_rmp_strategy(t_rmp_strategy),
      m_cut_template(Row(t_cut.row()), t_cut.type()),
      m_objective_template(t_cut.row().transpose()) {

    save_subproblem_ids();
    remove_cut_template_from_rmp(t_cut);

}

void CutGenerationSP::save_subproblem_ids() {
    for (const auto& [var, constant] : m_objective_template.lhs()) {
        m_subproblem_ids.emplace(var.model_id());
    }
}

void CutGenerationSP::remove_cut_template_from_rmp(const Ctr& t_cut) {
    EASY_LOG(Trace, "cut-generation", "Constraint " << t_cut << " has been removed from the RMP for it will be generated.");
    rmp_solution_strategy().remove_constraint(t_cut);
}

void CutGenerationSP::build() {

    if (!m_exact_solution_strategy) {
        throw Exception("No exact solution strategy has been given.");
    }

    m_exact_solution_strategy->build();
}

void CutGenerationSP::solve() {
    m_exact_solution_strategy->solve();
}

Row CutGenerationSP::get_separation_objective(const Solution::Primal &t_primals) const {
    double sign = m_cut_template.type() == LessOrEqual ? 1. : -1.;

    Row result;

    for (const auto& [var, constant] : m_objective_template.lhs()) {
        result.lhs() += sign * constant.fix(t_primals) * var;
    }

    result.rhs() = sign * m_objective_template.rhs().fix(t_primals);

    return result;
}

void CutGenerationSP::update_separation_objective(const Row &t_objective) {
    m_exact_solution_strategy->update_objective(t_objective);
}

void CutGenerationSP::save_last_primal_solution() {
    m_primal_solutions.emplace_back(std::make_unique<Solution::Primal>(m_exact_solution_strategy->primal_solution()));
}

void CutGenerationSP::log_last_primal_solution() {
    EASY_LOG(Debug, "cut-generation",
             std::setw(5)
             << "SP"
             << std::setw(15)
             << m_primal_solutions.back()->status()
             << std::setw(10)
             << m_primal_solutions.back()->objective_value()
    );
}

bool CutGenerationSP::violated_cut_found() {
    return m_primal_solutions.back()->objective_value() < -ToleranceForAbsoluteGapPricing;
}

void CutGenerationSP::add_cut_to_rmp() {
    auto* last_primal_solution = m_primal_solutions.back().get();
    auto temp_ctr = create_cut_from(*last_primal_solution);
    auto constraint = m_rmp_strategy.add_constraint(std::move(temp_ctr));
    m_currently_present_cuts.template emplace_back(constraint, *last_primal_solution);
    EASY_LOG(Trace, "cut-generation", "Adding new constraint " << constraint << ".");
}

bool CutGenerationSP::is_unbounded() const {
    return m_primal_solutions.back()->status() == Unbounded;
}

bool CutGenerationSP::is_infeasible() const {
    return m_primal_solutions.back()->status() == Infeasible;
}

bool CutGenerationSP::could_not_be_solved_to_optimality() const {
    return m_primal_solutions.back()->status() != Optimal;
}

TempCtr CutGenerationSP::create_cut_from(const Solution::Primal &t_primals) const {
    return { m_cut_template.row().fix(t_primals), m_cut_template.type() };
}

Solution::Primal CutGenerationSP::primal_solution() const {

    if (!m_original_space_builder) {
        return {};
    }

    return m_original_space_builder->primal_solution(*this, m_rmp_strategy);
}

bool CutGenerationSP::set_lower_bound(const Var &t_var, double t_lb) {
    if (!is_in_subproblem(t_var)) { return false; }

    remove_cuts_violating_lower_bound(t_var, t_lb);
    exact_solution_strategy().set_lower_bound(t_var, t_lb);

    return true;
}

bool CutGenerationSP::set_upper_bound(const Var &t_var, double t_ub) {
    if (!is_in_subproblem(t_var)) { return false; }

    remove_cuts_violating_upper_bound(t_var, t_ub);
    exact_solution_strategy().set_upper_bound(t_var, t_ub);

    return true;
}

void CutGenerationSP::remove_cut_if(const std::function<bool(const Ctr &, const Solution::Primal &)> &t_indicator_for_removal) {

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

bool CutGenerationSP::is_in_subproblem(const Var &t_var) const {
    return m_subproblem_ids.find(t_var.id()) != m_subproblem_ids.end();
}

void CutGenerationSP::remove_cuts_violating_lower_bound(const Var &t_var, double t_lb) {

    remove_cut_if([&](const Ctr& t_cut, const auto& t_cut_primal_solution){
        if (double value = t_cut_primal_solution.get(t_var) ; value < t_lb + ToleranceForIntegrality) {
            EASY_LOG(Trace,
                     "cut-generation",
                     "Cut " << t_cut << " was removed by contradiction with required "
                            << "bound " << t_var << " >= " << t_lb << " (" << t_var << " = " << value << ").");
            return true;
        }
        return false;
    });

}

void CutGenerationSP::remove_cuts_violating_upper_bound(const Var &t_var, double t_ub) {

    remove_cut_if([&](const Ctr& t_cut, const auto& t_cut_primal_solution){
        if (double value = t_cut_primal_solution.get(t_var) ; value > t_ub - ToleranceForIntegrality) {
            EASY_LOG(Trace,
                     "cut-generation",
                     "Cut " << t_cut << " was removed by contradiction with required "
                            << "bound " << t_var << " <= " << t_ub << " (" << t_var << " = " << value << ").");
            return true;
        }
        return false;
    });

}
