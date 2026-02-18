//
// Created by Henri on 21/01/2026.
//
#include "idol/robust/optimizers/column-and-constraint-generation/separation/Separation.h"
#include "idol/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

void idol::Robust::CCG::Separation::submit_upper_bound(double t_value) {

    if (!m_submitted_upper_bound || *m_submitted_upper_bound > t_value) {
        m_submitted_upper_bound = t_value;
    }

}

void idol::Robust::CCG::Separation::terminate() {
    m_parent->terminate();
}

void idol::Robust::CCG::Separation::set_status(SolutionStatus t_status, SolutionReason t_reason) {
    m_status = t_status;
    m_reason = t_reason;
}

const idol::PrimalPoint& idol::Robust::CCG::Separation::get_master_solution() const {
    return m_parent->get_last_master_solution();
}

idol::Env& idol::Robust::CCG::Separation::get_env() const {
    return m_parent->parent().env();
}

const idol::Robust::Description& idol::Robust::CCG::Separation::get_robust_description() const {
    return m_parent->get_robust_description();
}

const idol::Bilevel::Description& idol::Robust::CCG::Separation::get_bilevel_description() const {
    return m_parent->get_bilevel_description();
}

const idol::CCG::Formulation& idol::Robust::CCG::Separation::get_formulation() const {
    return m_parent->get_formulation();
}

std::pair<idol::Model, idol::Bilevel::Description>
idol::Robust::CCG::Separation::build_separation_problem() {

    auto& env = get_env();
    const auto& first_stage_solution = get_master_solution();
    const auto& robust_description = get_robust_description();
    const auto& bilevel_description = get_bilevel_description();
    const auto& formulation = get_formulation();
    const auto& original_model = formulation.original_model();
    const auto& original_objective_function = original_model.get_obj_expr();

    Model result_model(env);
    Bilevel::Description result_description(env, "separation");

    // Add uncertainty variables
    const auto& uncertainty_set = robust_description.uncertainty_set();
    for (const auto& var : uncertainty_set.vars()) {

        const double lb = uncertainty_set.get_var_lb(var);
        const double ub = uncertainty_set.get_var_ub(var);
        const auto type = uncertainty_set.get_var_type(var);

        result_model.add(var, TempVar(lb, ub, type, 0, LinExpr<Ctr>()));
        result_description.make_upper_level(var);

    }

    // Add uncertainty constraints
    for (const auto& ctr : uncertainty_set.ctrs()) {

        const auto& row = uncertainty_set.get_ctr_row(ctr);
        const auto type = uncertainty_set.get_ctr_type(ctr);
        double rhs = uncertainty_set.get_ctr_rhs(ctr);
        LinExpr<Var> copy(row); // If decision-dependent, this may change

        result_model.add(ctr, TempCtr(std::move(copy), type, rhs));
        result_description.make_upper_level(ctr);

    }

    // Add second-stage variables
    for (const auto& var : formulation.second_stage_variables()) {

        const double lb = original_model.get_var_lb(var);
        const double ub = original_model.get_var_ub(var);
        const auto type = original_model.get_var_type(var);

        result_model.add(var, TempVar(lb, ub, type, 0, LinExpr<Ctr>()));
        result_description.make_lower_level(var);

    }

    // Add second-stage constraints
    for (const auto& ctr : formulation.second_stage_constraints()) {

        const auto& row = original_model.get_ctr_row(ctr);
        const auto type = original_model.get_ctr_type(ctr);
        double rhs = original_model.get_ctr_rhs(ctr);

        LinExpr<Var> new_row;
        for (const auto& [var, coeff] : row) {
            if (bilevel_description.is_upper(var)) {
                rhs -= coeff * first_stage_solution.get(var);
                continue;
            }
            new_row += coeff * var;
        }

        for (const auto& [var, coeff] : robust_description.uncertain_rhs(ctr)) {
            new_row -= coeff * var;
        }

        for (const auto& [var, coeff] : robust_description.uncertain_mat_coeffs(ctr)) {
            if (bilevel_description.is_lower(var)) {
                throw Exception("Uncertain matrix coefficients cannot be handled.");
            }
            new_row += first_stage_solution.get(var) * coeff;
        }

        result_model.add(ctr, TempCtr(std::move(new_row), type, rhs));
        result_description.make_lower_level(ctr);

    }

    // Objective function
    QuadExpr<Var> objective = original_objective_function.affine().constant();
    for (const auto& [var, coeff] : original_objective_function.affine().linear()) {
        if (bilevel_description.is_upper(var)) {
            objective += coeff * first_stage_solution.get(var);
        } else {
            objective += coeff * var;
        }
    }
    result_model.set_obj_expr(-objective);
    result_description.set_lower_level_obj(std::move(objective));

    if (original_objective_function.has_quadratic()) {
        throw Exception("Uncertain quadratic objectives not yet implemented.");
    }

    if (!robust_description.uncertain_obj().empty()) {
        throw Exception("Uncertain objectives not yet implemented.");
    }

    return {
        std::move(result_model),
        std::move(result_description)
    };
}
