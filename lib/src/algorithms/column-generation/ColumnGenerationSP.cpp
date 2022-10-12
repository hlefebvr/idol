//
// Created by henri on 19/09/22.
//
#include "../../../include/algorithms/column-generation/ColumnGenerationSP.h"
#include "../../../include/algorithms/Algorithm.h"

ColumnGenerationSP::ColumnGenerationSP(Algorithm& t_rmp_strategy, const Var& t_var)
        : m_rmp_strategy(t_rmp_strategy),
          m_var_template(TempVar(t_var.lb(), t_var.ub(), t_var.type(), Column(t_var.column()))) {

    save_subproblem_ids(t_var);
    remove_var_template_from_rmp(t_var);

}

void ColumnGenerationSP::solve() {
    m_exact_solution_strategy->solve();
}


Solution::Primal ColumnGenerationSP::primal_solution() const {
    const auto rmp_primals = m_rmp_strategy.primal_solution();
    Solution::Primal result;

    double sum_primals = 0.;

    for (const auto& [var, primal_solution] : currently_present_variables()) {
        const double primal = rmp_primals.get(var);

        if (!equals(primal, 0., ToleranceForSparsity)) {
            result += primal * primal_solution;
            sum_primals += primal;
        }
    }

    if (!equals(sum_primals, 1., ToleranceForSparsity)) {
        result *= 1. / sum_primals;
    }

    return result;
}


Solution::Dual ColumnGenerationSP::dual_solution() const {
    return m_exact_solution_strategy->dual_solution();
}


Row ColumnGenerationSP::get_pricing_objective(const Solution::Dual &t_duals) const {

    Row result;

    if (t_duals.status() == Optimal) {
        for (const auto &[param, coeff]: m_var_template.column().objective_coefficient()) {
            result.lhs() += coeff * param.as<Var>();
        }
    }

    for (const auto &[ctr, constant]: m_var_template.column().components()) {
        result.rhs() += constant.numerical() * -t_duals.get(ctr);
        for (const auto &[param, coeff]: constant) {
            result.lhs() += -t_duals.get(ctr) * coeff * param.as<Var>();
        }
    }

    return result;
}


void ColumnGenerationSP::update_pricing_objective(const Row &t_objective) {
    m_exact_solution_strategy->update_objective(t_objective);
}


void ColumnGenerationSP::build() {

    if (!m_branching_scheme) {
        throw Exception("No branching scheme has been given.");
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

    return { m_var_template.lb(),
             m_var_template.ub(),
             m_var_template.type(),
             m_var_template.column().fix(t_primal_solution)
    };
}

void ColumnGenerationSP::remove_column_if(const std::function<bool(const Var&, const Solution::Primal &)> &t_indicator_for_removal) {

    auto it = m_currently_present_variables.begin();
    const auto end = m_currently_present_variables.end();

    while (it != end) {
        const auto& [column_variable, ptr_to_column] = *it;
        if (t_indicator_for_removal(column_variable, ptr_to_column)) {
            m_rmp_strategy.remove(column_variable);
            it = m_currently_present_variables.erase(it);
        } else {
            ++it;
        }
    }

}

bool ColumnGenerationSP::set_lower_bound(const Var &t_var, double t_lb) {

    if (!is_in_subproblem(t_var)) { return false; }

    m_branching_scheme->set_lower_bound(t_var, t_lb, *this);

    return true;
}

bool ColumnGenerationSP::set_upper_bound(const Var &t_var, double t_ub) {

    if (!is_in_subproblem(t_var)) { return false; }

    m_branching_scheme->set_upper_bound(t_var, t_ub, *this);

    return true;
}

std::optional<Ctr> ColumnGenerationSP::add_constraint(TempCtr &t_temporay_constraint) {
    return m_branching_scheme->contribute_to_add_constraint(t_temporay_constraint, *this);
}

bool ColumnGenerationSP::update_constraint_rhs(const Ctr &t_ctr, double t_rhs) {

    if (!is_in_subproblem(t_ctr)) { return false; }

    m_exact_solution_strategy->update_coefficient_rhs(t_ctr, t_rhs);

    remove_columns_violating_constraint(TempCtr(Row(t_ctr.row()), t_ctr.type()));

    return true;
}

bool ColumnGenerationSP::remove_constraint(const Ctr &t_ctr) {

    if (!is_in_subproblem(t_ctr)) {
        reset_linking_expr(t_ctr);
        return false;
    }

    EASY_LOG(Trace, "column-generation", "Constraint " << t_ctr << " has been removed from SP.");
    m_exact_solution_strategy->remove(t_ctr);

    return true;
}

void ColumnGenerationSP::reset_linking_expr(const Ctr &t_ctr) {
    m_var_template.column().components().set(t_ctr, 0.);
}

void ColumnGenerationSP::add_linking_expr(const Ctr &t_ctr, const Expr<Var> &t_expr) {
    Constant value;
    for (const auto& [var, constant] : t_expr) {
        value += constant.numerical() * !var;
    }

    m_var_template.column().components().set(t_ctr, value);
}

void ColumnGenerationSP::save_subproblem_ids(const Var& t_var) {

    for (const auto& [ctr, constant] : t_var.column().components()) {
        for (const auto& [param, coeff] : constant) {
            m_subproblem_ids.emplace(param.model_id());
            m_subproblem_ids.emplace(param.model_id());
        }
    }

}

void ColumnGenerationSP::remove_var_template_from_rmp(const Var &t_var) {
    EASY_LOG(Trace, "column-generation", "Variable " << t_var << " has been removed from the RMP for it will be generated.");
    rmp_solution_strategy().remove(t_var);
}

bool ColumnGenerationSP::is_in_subproblem(const Var &t_var) const {
    return m_subproblem_ids.find(t_var.model_id()) != m_subproblem_ids.end();
}

bool ColumnGenerationSP::is_in_subproblem(const Ctr &t_ctr) const {
    return m_subproblem_ids.find(t_ctr.model_id()) != m_subproblem_ids.end();
}

void ColumnGenerationSP::remove_columns_violating_lower_bound(const Var &t_var, double t_lb) {
    remove_column_if([&](const Var& t_column_variable, const auto& t_column_primal_solution){
        if (double value = t_column_primal_solution.get(t_var) ; value < t_lb + ToleranceForIntegrality) {
            EASY_LOG(Trace,
                     "column-generation",
                     "Column " << t_column_variable << " was removed by contradiction with required "
                               << "bound " << t_var << " >= " << t_lb << " (" << t_var << " = " << value << ").");
            return true;
        }
        return false;
    });
}

void ColumnGenerationSP::remove_columns_violating_upper_bound(const Var &t_var, double t_ub) {
    remove_column_if([&](const Var& t_column_variable, const auto& t_column_primal_solution){
        if (double value = t_column_primal_solution.get(t_var) ; value > t_ub - ToleranceForIntegrality) {
            EASY_LOG(Trace,
                     "column-generation",
                     "Column " << t_column_variable << " was removed by contradiction with required "
                               << "bound " << t_var << " <= " << t_ub << " (" << t_var << " = " << value << ").");
            return true;
        }
        return false;
    });
}

void ColumnGenerationSP::remove_columns_violating_constraint(const TempCtr &t_ctr) {
    remove_column_if([&](const Var& t_column_variable, const Solution::Primal& t_solution) {
        if (t_ctr.is_violated(t_solution)) {
            EASY_LOG(Trace,
                     "column-generation",
                     "Column " << t_column_variable << " was removed by contradiction with required "
                               << "constraint " << t_ctr << '.');
            return true;
        }
        return false;
    });
}

Expr<Var> ColumnGenerationSP::expand(const Var &t_subproblem_variable) const {
    Expr result;
    for (const auto& [var, column] : m_currently_present_variables) {
        result += column.get(t_subproblem_variable) * var;
    }
    return result;
}
