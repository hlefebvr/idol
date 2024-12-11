//
// Created by henri on 11.12.24.
//
#include <idol/robust/optimizers/column-and-constraint-generation/Formulation.h>
#include <idol/mixed-integer/modeling/variables/TempVar.h>
#include <idol/mixed-integer/modeling/expressions/operations/operators.h>

idol::CCG::Formulation::Formulation(const Model &t_parent, const ::idol::Robust::Description &t_description)
    : m_parent(t_parent), m_description(t_description), m_master(t_parent.env()) {

    parse_variables();
    parse_objective();
    parse_constraints();

}

void idol::CCG::Formulation::parse_variables() {

    for (const auto& var : m_parent.vars()) {

        if (m_description.stage(var) > 0) {
            m_second_stage_variables.emplace_back(var);
            continue;
        }

        const double lb = m_parent.get_var_lb(var);
        const double ub = m_parent.get_var_ub(var);
        const auto type = m_parent.get_var_type(var);
        const double obj = m_parent.get_var_obj(var);

        m_master.add(var, TempVar(lb, ub, type, obj, LinExpr<Ctr>()));

    }

}

void idol::CCG::Formulation::parse_objective() {

    const auto& objective = m_parent.get_obj_expr();

    if (!objective.has_quadratic()) {
        return; // Because the objective has been added while parsing the variables
    }

    auto master_objective = m_master.get_obj_expr();

    for (const auto& [pair, coeff] : objective) {

        if (m_description.stage(pair.first) > 0) {
            continue;
        }

        if (m_description.stage(pair.second) > 0) {
            continue;
        }

        master_objective += coeff * pair.first * pair.second;

    }

}

void idol::CCG::Formulation::parse_constraints() {

    for (const auto& ctr : m_parent.ctrs()) {

        const auto& row = m_parent.get_ctr_row(ctr);

        const bool has_first_stage = std::any_of(row.begin(), row.end(), [this](const auto& term) {
            return m_description.stage(term.first) == 0;
        });
        const bool has_second_stage = std::any_of(row.begin(), row.end(), [this](const auto& term) {
            return m_description.stage(term.first) > 0;
        });

        if (has_second_stage || !m_description.uncertain_mat_coeffs(ctr).empty()) {
            if (has_first_stage) {
                m_linking_constraints.emplace_back(ctr);
            }
            m_second_stage_constraints.emplace_back(ctr);
            continue;
        }

        const auto type = m_parent.get_ctr_type(ctr);
        const double rhs = m_parent.get_ctr_rhs(ctr);

        m_master.add(ctr, TempCtr(LinExpr(row), type, rhs));
    }

}

void idol::CCG::Formulation::add_scenario_to_master(const idol::Point<idol::Var> &t_scenario) {

    std::vector<std::optional<Var>> new_vars;
    new_vars.resize(m_parent.vars().size());

    // Add Variables
    for (const auto& var : m_second_stage_variables) {

        const double lb = m_parent.get_var_lb(var);
        const double ub = m_parent.get_var_ub(var);
        const auto type = m_parent.get_var_type(var);
        const auto index = m_parent.get_var_index(var);

        new_vars[index] = m_master.add_var(lb, ub, type, 0, var.name() + "_" + std::to_string(m_n_added_scenario));

    }

    // Add Constraints
    for (const auto& ctr : m_second_stage_constraints) {

        const auto& row = m_parent.get_ctr_row(ctr);
        const auto type = m_parent.get_ctr_type(ctr);
        double rhs = m_parent.get_ctr_rhs(ctr);

        LinExpr<Var> new_row;
        for (const auto& [var, coeff] : row) {

            if (m_description.stage(var) == 0) {
                new_row += coeff * var;
                continue;
            }

            new_row += coeff * new_vars[m_parent.get_var_index(var)].value();
        }

        const auto uncertainties = m_description.uncertain_mat_coeffs(ctr);
        for (const auto& [var, coeff] : uncertainties) {
            new_row += evaluate(coeff, t_scenario) * new_vars[m_parent.get_var_index(var)].value();
        }

        rhs += evaluate(m_description.uncertain_rhs(ctr), t_scenario);

        m_master.add_ctr(TempCtr(std::move(new_row), type, rhs), ctr.name() + "_" + std::to_string(m_n_added_scenario));

    }

    if (!m_second_stage_epigraph) {
        m_second_stage_epigraph = m_master.add_var(-Inf, Inf, Continuous, 1, "second_stage_epigraph");
    }

    // Add Objective
    LinExpr<Var> objective;
    for (const auto& var : m_second_stage_variables) {
        objective += m_parent.get_var_obj(var) * new_vars[m_parent.get_var_index(var)].value();
    }
    for (const auto& [var, coeff] : m_description.uncertain_obj()) {
        objective += evaluate(coeff, t_scenario) * new_vars[m_parent.get_var_index(var)].value();
    }

    m_master.add_ctr(*m_second_stage_epigraph >= std::move(objective));

    ++m_n_added_scenario;
}
