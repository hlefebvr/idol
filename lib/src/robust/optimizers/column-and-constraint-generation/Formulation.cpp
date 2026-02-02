//
// Created by henri on 11.12.24.
//
#include <idol/robust/optimizers/column-and-constraint-generation/Formulation.h>
#include <idol/mixed-integer/modeling/variables/TempVar.h>
#include <idol/mixed-integer/modeling/expressions/operations/operators.h>
#include <cassert>
#include "idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h"


idol::CCG::Formulation::Formulation(const idol::Model &t_parent, const idol::Robust::Description &t_robust_description,
                                    const idol::Bilevel::Description &t_bilevel_description)
        : m_parent(t_parent),
          m_robust_description(t_robust_description),
          m_bilevel_description(t_bilevel_description),
          m_master(t_parent.env()) {

    parse_variables();
    parse_objective();
    parse_constraints();

    if (!m_coupling_constraints.empty()) {
        throw Exception("Coupling constraints make no sense for two-stage robust problems and are not supported for robust bilevel problems.");
    }

    if (is_wait_and_see_follower()) {
        auto& env = m_parent.env();
        m_bilevel_description_master = std::make_optional<::idol::Bilevel::Description>(env);
        copy_bilevel_description(t_bilevel_description, *m_bilevel_description_master);
    }

}

void idol::CCG::Formulation::parse_variables() {

    for (const auto& var : m_parent.vars()) {

        const double obj = m_parent.get_var_obj(var);

        if (m_bilevel_description.is_lower(var)) {
            m_second_stage_variables.emplace_back(var);
            if (!is_zero(obj, Tolerance::Sparsity)) {
                m_has_second_stage_objective = true;
            }
            continue;
        }

        const double lb = m_parent.get_var_lb(var);
        const double ub = m_parent.get_var_ub(var);
        const auto type = m_parent.get_var_type(var);

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

        if (m_bilevel_description.is_lower(pair.first)) {
            m_has_second_stage_objective = true;
            continue;
        }

        if (m_bilevel_description.is_lower(pair.second)) {
            m_has_second_stage_objective = true;
            continue;
        }

        master_objective += coeff * pair.first * pair.second;

    }

}

void idol::CCG::Formulation::parse_constraints() {

    for (const auto& ctr : m_parent.ctrs()) {

        const auto& row = m_parent.get_ctr_row(ctr);

        const bool has_first_stage = std::any_of(row.begin(), row.end(), [this](const auto& term) {
            return m_bilevel_description.is_upper(term.first);
        });
        const bool has_second_stage = std::any_of(row.begin(), row.end(), [this](const auto& term) {
            return m_bilevel_description.is_lower(term.first);
        });

        if (has_second_stage || !m_robust_description.uncertain_mat_coeffs(ctr).empty()) {

            if (has_first_stage) {

                // A coupling constraint is an upper-level constraint with a lower-level variable or with uncertainty
                if (m_bilevel_description.is_upper(ctr)) {
                    m_coupling_constraints.emplace_back(ctr);
                } else {
                    m_linking_constraints.emplace_back(ctr);
                }

            }

            m_second_stage_constraints.emplace_back(ctr);
            continue;
        }

        const auto type = m_parent.get_ctr_type(ctr);
        const double rhs = m_parent.get_ctr_rhs(ctr);

        m_master.add(ctr, TempCtr(LinExpr(row), type, rhs));
    }

}

void idol::CCG::Formulation::add_scenario_to_master(const idol::Point<idol::Var> &t_scenario, bool t_add_annotation, bool t_check_for_repeated_scenarios) {

    const auto& lower_level_annotation = m_bilevel_description_master.has_value() ? m_bilevel_description_master->lower_level() : m_bilevel_description.lower_level();

    std::vector<std::optional<Var>> new_vars;
    new_vars.resize(m_parent.vars().size());

    // Add Variables
    for (const auto& var : m_second_stage_variables) {

        const double lb = m_parent.get_var_lb(var);
        const double ub = m_parent.get_var_ub(var);
        const auto type = m_parent.get_var_type(var);
        const auto index = m_parent.get_var_index(var);

        new_vars[index] = m_master.add_var(lb, ub, type, 0, var.name() + "_" + std::to_string(m_n_added_scenario));
        if (t_add_annotation) {
            new_vars[index]->set(lower_level_annotation, m_n_added_scenario);
        }
    }

    m_master.update();

    // Add Constraints
    for (const auto& ctr : m_second_stage_constraints) {

        const auto& row = m_parent.get_ctr_row(ctr);
        const auto type = m_parent.get_ctr_type(ctr);
        double rhs = m_parent.get_ctr_rhs(ctr);

        LinExpr<Var> new_row;
        for (const auto& [var, coeff] : row) {

            if (m_bilevel_description.is_upper(var)) {
                new_row += coeff * var;
                continue;
            }

            new_row += coeff * new_vars[m_parent.get_var_index(var)].value();
        }

        const auto uncertainties = m_robust_description.uncertain_mat_coeffs(ctr);
        for (const auto& [var, coeff] : uncertainties) {
            const double coeff_value = evaluate(coeff, t_scenario);
            if (m_bilevel_description.is_upper(var)) {
                new_row += coeff_value * var;
            } else {
                new_row += coeff_value * new_vars[m_parent.get_var_index(var)].value();
            }
        }

        rhs += evaluate(m_robust_description.uncertain_rhs(ctr), t_scenario);

        const auto new_ctr = m_master.add_ctr(TempCtr(std::move(new_row), type, rhs), ctr.name() + "_" + std::to_string(m_n_added_scenario));
        if (t_add_annotation) {
            new_ctr.set(lower_level_annotation, m_n_added_scenario);
        }

    }

    if (!m_second_stage_epigraph) {
        add_epigraph_to_master();
    }

    // Add Objective
    LinExpr<Var> objective;
    for (const auto& var : m_second_stage_variables) {
        objective += m_parent.get_var_obj(var) * new_vars[m_parent.get_var_index(var)].value();
    }
    for (const auto& [var, coeff] : m_robust_description.uncertain_obj()) {
        objective += evaluate(coeff, t_scenario) * new_vars[m_parent.get_var_index(var)].value();
    }

    m_master.add_ctr(*m_second_stage_epigraph >= std::move(objective));

    if (is_wait_and_see_follower() && t_add_annotation) {

        auto lower_objective = m_bilevel_description_master->lower_level_obj();

        for (const auto& [var, coeff] : m_bilevel_description.lower_level_obj().affine().linear()) {
            if (m_bilevel_description.is_upper(var)) {
                continue;
            }
            lower_objective += coeff * new_vars[m_parent.get_var_index(var)].value();
        }

        m_bilevel_description_master->set_lower_level_obj(std::move(lower_objective));

        m_master.update();

    }

    ++m_n_added_scenario;

    if (t_check_for_repeated_scenarios) {

        for (const auto& scenario : m_generated_scenarios) {
            if ((scenario - t_scenario).is_zero(1e-3)) {
                std::cerr << "Repeated scenario!" << std::endl;
                std::cerr << "Already present:\n" << scenario << std::endl;
                std::cerr << "Newly separated:\n" << t_scenario << std::endl;
                throw Exception("Reapeated scenario!");
            }
        }

        m_generated_scenarios.emplace_back(t_scenario);
    }
}

void idol::CCG::Formulation::copy_bilevel_description(const ::idol::Bilevel::Description& t_src, const ::idol::Bilevel::Description& t_dest) const {

    const auto& src_annotation = t_src.lower_level();
    const auto& dest_annotation = t_dest.lower_level();

    for (const auto& var : m_parent.vars()) {
        var.set(dest_annotation, var.get(src_annotation));
    }

    for (const auto& ctr : m_parent.ctrs()) {
        ctr.set(dest_annotation, ctr.get(src_annotation));
    }

}

bool idol::CCG::Formulation::is_adjustable_robust_problem() const {
    return m_bilevel_description.lower_level_obj().is_zero(Tolerance::Feasibility);
}

idol::QuadExpr<idol::Var>
idol::CCG::Formulation::compute_second_stage_objective(const idol::Point<idol::Var> &t_first_stage_decision) const {

    QuadExpr<Var> result;

    const auto& src_objective = m_parent.get_obj_expr();

    result += src_objective.affine().constant();
    for (const auto& [var, coeff] : src_objective.affine().linear()) {
        if (m_bilevel_description.is_upper(var)) {
            result += coeff * t_first_stage_decision.get(var);
            continue;
        }
        result += coeff * var;
    }

    if (src_objective.has_quadratic()) {
        throw Exception("Quadratic objectives not yet implemented.");
    }

    return result;
}

bool idol::CCG::Formulation::should_have_epigraph_and_epigraph_is_not_in_master() const {
    return m_has_second_stage_objective && (!m_second_stage_epigraph || !m_master.has(*m_second_stage_epigraph));
}

void idol::CCG::Formulation::add_epigraph_to_master() {
    m_second_stage_epigraph = m_master.add_var(-Inf, Inf, Continuous, 1, "second_stage_epigraph");
    //m_master.update();
}
