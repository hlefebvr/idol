//
// Created by henri on 05/10/22.
//
#include <iomanip>
#include "../../../include/algorithms/row-generation/RowGenerationSP.h"
#include "../../../include/algorithms/logs/Log.h"
#include "../../../include/modeling/expressions/operators.h"

RowGenerationSP::RowGenerationSP(Algorithm &t_rmp_strategy, const Ctr& t_cut)
    : m_rmp_strategy(t_rmp_strategy),
      m_cut_template(Row(t_rmp_strategy.get_row(t_cut)), t_rmp_strategy.get_type(t_cut)) {

    if (m_cut_template.type() == Equal) {
        throw Exception("Cannot separate equality constraints.");
    }

    remove_cut_template_from_rmp(t_cut);

}

void RowGenerationSP::remove_cut_template_from_rmp(const Ctr& t_cut) {
    EASY_LOG(Trace, "row-generation", "Constraint " << t_cut << " has been removed from the RMP for it will be generated.");
    rmp_solution_strategy().remove(t_cut);
}

void RowGenerationSP::initialize() {

    if (!m_exact_solution_strategy) {
        throw Exception("No exact solution strategy has been given.");
    }

    /*
    for (const auto& [ctr, primal_solution] : m_currently_present_cuts) {
        rmp_solution_strategy().remove(ctr);
    }
    m_pool.clear();
    m_urrently_present_cuts.clear();
     */

    /*
    for (auto& [ctr, primal_solution] : m_pool.values()) {
        if (ctr.status() == InModel) { continue; }
        bool is_feasible = true;
        for (const auto& [var, value] : primal_solution) {
            if (!(var.lb() <= value && value <= var.ub())) {
                is_feasible = false;
                break;
            }
        }
        if (is_feasible) {
            ctr = m_rmp_strategy.add_ctr(create_cut_from(primal_solution));
            m_currently_present_cuts.emplace_back(ctr, primal_solution);
        }
    }
     */

}

void RowGenerationSP::solve() {
    m_exact_solution_strategy->solve();
}

Expr<Var> RowGenerationSP::get_separation_objective(const Solution::Primal &t_primals) const {

    double sign = m_cut_template.type() == LessOrEqual ? 1. : -1.;

    Expr result = sign * m_cut_template.row().rhs().numerical();

    for (const auto& [param, coeff] : m_cut_template.row().rhs()) {
        result += sign * coeff * param.as<Var>();
    }

    for (const auto &[var, constant]: m_cut_template.row().lhs().linear()) {
        result += -sign * constant.numerical() * t_primals.get(var);
        for (const auto &[param, coeff]: constant) {
            result += -sign * t_primals.get(var) * coeff * param.as<Var>();
        }
    }

    return result;
}

void RowGenerationSP::update_separation_objective(const Expr<Var> &t_objective) {
    m_exact_solution_strategy->update_obj(t_objective);
}

void RowGenerationSP::save_last_primal_solution() {
    m_last_primal_solution = m_exact_solution_strategy->primal_solution();

    std::unordered_map<std::string, double> values;

    for (const auto& [var, val] : m_last_primal_solution.value()) {
        values.emplace(var.name(), val);
    }

    for (unsigned int j = 0 ; j < 30 ; ++j) {
        const double y_j = values["y_" + std::to_string(j) + "_0"];
        const double z_j = values["z_" + std::to_string(j) + "_0"];
        if (y_j < .5 && z_j > .5) {
            std::cout << y_j << ", " << z_j << std::endl;
        }
    }
}

void RowGenerationSP::log_last_primal_solution() {
    EASY_LOG(Debug, "row-generation",
             std::setw(5)
                     << "SP"
                     << std::setw(15)
                     << m_last_primal_solution.value().status()
                     << std::setw(15)
                     << m_last_primal_solution.value().reason()
                     << std::setw(10)
                     << m_last_primal_solution.value().objective_value()
    );
}

bool RowGenerationSP::violated_cut_found() {
    return m_last_primal_solution.value().objective_value() < -ToleranceForAbsoluteGapPricing;
}

void RowGenerationSP::add_cut_to_rmp() {
    auto& last_primal_solution = m_last_primal_solution.value();
    auto temp_ctr = create_cut_from(last_primal_solution);
    auto constraint = m_rmp_strategy.add_ctr(std::move(temp_ctr));
    m_pool.add(constraint, std::move(m_last_primal_solution).value());
    m_currently_present_cuts.emplace_back(constraint, m_pool.last_inserted());
    EASY_LOG(Trace, "row-generation", "Adding new constraint " << constraint << ".");
}

bool RowGenerationSP::is_unbounded() const {
    return m_last_primal_solution.value().status() == Unbounded;
}

bool RowGenerationSP::is_infeasible() const {
    return m_last_primal_solution.value().status() == Infeasible;
}

bool RowGenerationSP::could_not_be_solved_to_optimality() const {
    return m_last_primal_solution.value().status() != Optimal;
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
    if (!exact_solution_strategy().has(t_var)) { return false; }

    remove_cuts_violating_lower_bound(t_var, t_lb);
    exact_solution_strategy().update_var_lb(t_var, t_lb);

    return true;
}

bool RowGenerationSP::set_upper_bound(const Var &t_var, double t_ub) {
    if (!exact_solution_strategy().has(t_var)) { return false; }

    remove_cuts_violating_upper_bound(t_var, t_ub);
    exact_solution_strategy().update_var_ub(t_var, t_ub);

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
