//
// Created by henri on 19/09/22.
//
#include "../../../include/algorithms/column-generation/ColumnGenerationSP.h"
#include "../../../include/modeling/models/Model.h"
#include "../../../include/modeling/expressions/operations/operators.h"
#include "../../../include/algorithms/Algorithm.h"

ColumnGenerationSP::ColumnGenerationSP(Algorithm& t_rmp_strategy, const Var& t_var)
        : m_rmp_strategy(t_rmp_strategy),
          m_var_template(TempVar(
                  t_rmp_strategy.get_lb(t_var),
                  t_rmp_strategy.get_ub(t_var),
                  t_rmp_strategy.get_type(t_var),
                  Column(t_rmp_strategy.get_column(t_var)))
              ) {

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


Expr<Var> ColumnGenerationSP::get_pricing_objective(const Solution::Dual &t_duals) const {

    Expr<Var> result;

    if (t_duals.status() == Optimal) {
        for (const auto &[param, coeff]: m_var_template.column().objective_coefficient()) {
            result += coeff * param.as<Var>();
        }
    }

    for (const auto &[ctr, constant]: m_var_template.column().components().linear()) {
        result += constant.numerical() * -t_duals.get(ctr);
        for (const auto &[param, coeff]: constant) {
            result += -t_duals.get(ctr) * coeff * param.as<Var>();
        }
    }

    return result;
}


void ColumnGenerationSP::update_pricing_objective(const Expr<Var> &t_objective) {
    m_exact_solution_strategy->update_obj(t_objective);
}


void ColumnGenerationSP::initialize() {

    if (!m_branching_scheme) {
        throw Exception("No branching scheme has been given.");
    }

    if (!m_exact_solution_strategy) {
        throw Exception("No exact solution strategy has been given.");
    }

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
    idol_Log(Debug, "column-generation",
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
    auto variable = m_rmp_strategy.add_var(std::move(temp_var));
    m_currently_present_variables.template emplace_back(variable, *last_primal_solution);
    idol_Log(Trace, "column-generation", "Adding new variable with name " << variable << ".");
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

    if (!exact_solution_strategy().has(t_var)) { return false; }

    m_branching_scheme->set_lower_bound(t_var, t_lb, *this);

    return true;
}

bool ColumnGenerationSP::set_upper_bound(const Var &t_var, double t_ub) {

    if (!exact_solution_strategy().has(t_var)) { return false; }

    m_branching_scheme->set_upper_bound(t_var, t_ub, *this);

    return true;
}

std::optional<Ctr> ColumnGenerationSP::add_constraint(TempCtr &t_temporay_constraint) {
    return m_branching_scheme->contribute_to_add_constraint(t_temporay_constraint, *this);
}

bool ColumnGenerationSP::update_constraint_rhs(const Ctr &t_ctr, double t_rhs) {

    if (!exact_solution_strategy().has(t_ctr)) { return false; }

    m_exact_solution_strategy->update_rhs_coeff(t_ctr, t_rhs);

    remove_columns_violating_constraint(TempCtr(Row(m_exact_solution_strategy->get_row(t_ctr)), m_exact_solution_strategy->get_type(t_ctr)));

    return true;
}

bool ColumnGenerationSP::remove_constraint(const Ctr &t_ctr) {

    if (!exact_solution_strategy().has(t_ctr)) {
        reset_linking_expr(t_ctr);
        return false;
    }

    idol_Log(Trace, "column-generation", "Constraint " << t_ctr << " has been removed from SP.");
    m_exact_solution_strategy->remove(t_ctr);

    return true;
}

void ColumnGenerationSP::reset_linking_expr(const Ctr &t_ctr) {
    m_var_template.column().components().linear().set(t_ctr, 0.);
}

void ColumnGenerationSP::add_linking_expr(const Ctr &t_ctr, const LinExpr<Var> &t_expr) {
    Constant value;
    for (const auto& [var, constant] : t_expr) {
        value += constant.numerical() * !var;
    }

    m_var_template.column().components().linear().set(t_ctr, value);
}

void ColumnGenerationSP::remove_var_template_from_rmp(const Var &t_var) {
    idol_Log(Trace, "column-generation", "Variable " << t_var << " has been removed from the RMP for it will be generated.");
    rmp_solution_strategy().remove(t_var);
}

void ColumnGenerationSP::remove_columns_violating_lower_bound(const Var &t_var, double t_lb) {
    remove_column_if([&](const Var& t_column_variable, const auto& t_column_primal_solution){
        if (double value = t_column_primal_solution.get(t_var) ; value < t_lb + ToleranceForIntegrality) {
            idol_Log(Trace,
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
            idol_Log(Trace,
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
            idol_Log(Trace,
                     "column-generation",
                     "Column " << t_column_variable << " was removed by contradiction with required "
                               << "constraint " << t_ctr << '.');
            return true;
        }
        return false;
    });
}

LinExpr<Var> ColumnGenerationSP::expand(const Var &t_subproblem_variable) const {
    LinExpr result;
    for (const auto& [var, column] : m_currently_present_variables) {
        result += column.get(t_subproblem_variable) * var;
    }
    return result;
}
