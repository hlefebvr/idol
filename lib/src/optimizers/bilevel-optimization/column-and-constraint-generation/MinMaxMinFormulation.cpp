//
// Created by henri on 04.03.24.
//
#include <cassert>
#include "idol/optimizers/bilevel-optimization/column-and-constraint-generation/MinMaxMinFormulation.h"
#include "idol/optimizers/bilevel-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/modeling/objects/Versions.h"
#include "idol/modeling/expressions/operations/operators.h"
#include "idol/modeling/models/dualize.h"

idol::Bilevel::impl::MinMaxMinFormulation::MinMaxMinFormulation(const idol::Optimizers::Bilevel::ColumnAndConstraintGeneration &t_parent,
                                                                const Annotation<Var, unsigned int>& t_variable_stage,
                                                                const Annotation<Ctr, unsigned int>& t_constraint_stage,
                                                                double t_penalty_parameter)
    : m_parent(t_parent),
      m_uncertainty_set(t_parent.parent().env()),
      m_second_stage_dual(t_parent.parent().env()),
      m_extended_lower_level_problem(t_parent.parent().env()),
      m_two_stage_robust_formulation(t_parent.parent().copy()),
      m_penalty_parameter(t_penalty_parameter),
      m_variable_stage(t_variable_stage),
      m_constraint_stage(t_constraint_stage)
    {

    identify_complicating_variables();
    auto coupling_variables = identify_coupling_variables();
    identify_easy_constraints();
    set_second_stage_dual_objective(coupling_variables);
    m_uncertainty_set.set_obj_expr(0);
    fill_two_stage_robust_formulation();

    build_extended_lower_level_problem(coupling_variables);

}

void idol::Bilevel::impl::MinMaxMinFormulation::identify_complicating_variables() {

    const auto& high_point_relaxation = m_parent.parent();
    const auto& var_annotation = m_parent.lower_level_variables();
    Constant second_stage_dual_obj;

    for (const auto& var : m_parent.parent().vars()) {

        if (var.get(var_annotation) == MasterId) {
            continue;
        }

        const double lb = high_point_relaxation.get_var_lb(var);
        const double ub = high_point_relaxation.get_var_ub(var);
        const VarType type = high_point_relaxation.get_var_type(var);

        // Complicating variables are those which are integer or appear in a quadratic term
        // All others are moved in the second stage dual
        if (high_point_relaxation.get_var_column(var).quadratic().empty() && high_point_relaxation.get_var_type(var) == Continuous) {
            m_second_stage_dual.add(var, TempVar(lb, ub, type, Column(0)));
            continue;
        }

        m_uncertainty_set.add(var, TempVar(lb, ub, type, Column(0)));

    }

}

std::list<idol::Var> idol::Bilevel::impl::MinMaxMinFormulation::identify_coupling_variables() {

    std::list<Var> result;

    const auto& high_point_relaxation = m_parent.parent();
    const auto& ctr_annotation = m_parent.lower_level_constraints();
    const auto& var_annotation = m_parent.lower_level_variables();

    for (const auto& ctr : high_point_relaxation.ctrs()) {

        if (ctr.get(ctr_annotation) == MasterId) {
            continue;
        }

        const auto& row = high_point_relaxation.get_ctr_row(ctr);

        for (const auto& [var, coefficient] : row.linear()) {

            if (var.get(var_annotation) != MasterId || m_second_stage_dual.has(var)) {
                continue;
            }

            const auto type = high_point_relaxation.get_var_type(var);
            const double lb = high_point_relaxation.get_var_lb(var);
            const double ub = high_point_relaxation.get_var_ub(var);
            const auto& obj = high_point_relaxation.get_var_column(var).obj();

            if (type != Binary) {
                throw Exception("Coupling variables must be binary");
            }

            m_second_stage_dual.add(var, TempVar(std::max(lb, 0.), std::min(ub, 1.), Continuous, Column(0)));
            result.emplace_back(var);

        }

    }

    return result;
}

void idol::Bilevel::impl::MinMaxMinFormulation::identify_easy_constraints() {

    const auto& high_point_relaxation = m_parent.parent();
    const auto& ctr_annotation = m_parent.lower_level_constraints();

    for (const auto& ctr : high_point_relaxation.ctrs()) {

        if (ctr.get(ctr_annotation) == MasterId) {
            continue;
        }

        const auto &row = high_point_relaxation.get_ctr_row(ctr);

        if (all_variables_are_in_the_uncertainty_set(row)) {
            const auto type = high_point_relaxation.get_ctr_type(ctr);
            m_uncertainty_set.add(ctr, TempCtr(Row(row), type));
            continue;
        }

        add_ctr_to_second_stage_dual(ctr, row);

    }

}

bool idol::Bilevel::impl::MinMaxMinFormulation::all_variables_are_in_the_uncertainty_set(const idol::Row &t_row) const {

    for (const auto& [var, coefficient] : t_row.linear()) {
        if (!m_uncertainty_set.has(var)) {
            return false;
        }
    }

    for (const auto& [var1, var2, coefficient] : t_row.quadratic()) {
        if (!m_uncertainty_set.has(var1) || !m_uncertainty_set.has(var2) ) {
            return false;
        }
    }

    return true;
}

void idol::Bilevel::impl::MinMaxMinFormulation::add_ctr_to_second_stage_dual(const idol::Ctr &t_ctr,
                                                                             const idol::Row &t_row) {

    const auto& high_point_relaxation = m_parent.parent();

    Expr lhs;
    Constant rhs = t_row.rhs().numerical();

    for (const auto& [param, coefficient] : t_row.rhs().linear()) {
        lhs -= coefficient * param.as<Var>();
    }

    for (const auto& [var, coefficient] : t_row.linear()) {

        if (m_uncertainty_set.has(var)) {
            rhs -= coefficient.as_numerical() * !var;
            continue;
        }

        lhs += coefficient * var;

    }

    for (const auto& [var1, var2, coefficient] : t_row.quadratic()) {

        if (m_uncertainty_set.has(var1) && m_uncertainty_set.has(var2)) {
            rhs -= coefficient.as_numerical() * (!var1 * !var2);
            continue;
        }

        assert(!m_uncertainty_set.has(var1) && !m_uncertainty_set.has(var2));

        lhs += coefficient * var1 * var2;

    }

    const auto type = high_point_relaxation.get_ctr_type(t_ctr);

    m_second_stage_dual.add(t_ctr, TempCtr(Row(lhs, rhs), type));

}

void idol::Bilevel::impl::MinMaxMinFormulation::set_second_stage_dual_objective(const std::list<idol::Var> &t_coupling_variables) {

    const auto& high_point_relaxation = m_parent.parent();
    const auto& lower_level_objective = high_point_relaxation.get_ctr_row(m_parent.lower_level_objective());

    Expr objective = lower_level_objective.rhs();

    for (const auto& [var, constant] : lower_level_objective.linear()) {

        if (m_uncertainty_set.has(var)) {
            objective += constant.as_numerical() * !var;
        } else {
            objective += constant * var;
        }

    }

    m_second_stage_dual.set_obj_expr(
            objective +
            idol_Sum(var, t_coupling_variables, m_penalty_parameter * (!var + var - 2 * !var * var))
            );

}

void idol::Bilevel::impl::MinMaxMinFormulation::fill_two_stage_robust_formulation() {

    const auto& high_point_relaxation = m_parent.parent();

    m_two_stage_robust_formulation.unuse();

    m_two_stage_robust_formulation.remove(m_parent.lower_level_objective());

    const auto second_stage_primal = dualize(m_second_stage_dual);

    for (const auto& var : second_stage_primal.vars()) {

        const double lb = second_stage_primal.get_var_lb(var);
        const double ub = second_stage_primal.get_var_ub(var);
        const VarType type = second_stage_primal.get_var_type(var);

        m_two_stage_robust_formulation.add(var, TempVar(lb, ub, type, Column(0)));

        var.set(m_variable_stage, 0);

    }

    for (const auto& ctr : second_stage_primal.ctrs()) {

        const auto& row = second_stage_primal.get_ctr_row(ctr);
        const auto type = second_stage_primal.get_ctr_type(ctr);

        assert(row.quadratic().empty());

        const auto& lhs = row.linear();
        auto rhs = to_two_stage_robust_formulation_space(row.rhs());

        m_two_stage_robust_formulation.add(ctr, TempCtr(Row(lhs, rhs), type));

        ctr.set(m_constraint_stage, 0);
    }

    const auto& second_stage_dual_objective = second_stage_primal.get_obj_expr();

    assert(second_stage_dual_objective.quadratic().empty());

    Expr objective = to_two_stage_robust_formulation_space(second_stage_dual_objective.constant());

    for (const auto& [var, coefficient] : second_stage_dual_objective.linear()) {

        objective += coefficient * var;

    }

    const auto& lower_level_objective_ctr = high_point_relaxation.get_ctr_row(m_parent.lower_level_objective());
    const auto lower_level_objective = lower_level_objective_ctr.linear() - lower_level_objective_ctr.rhs();

    m_two_stage_robust_formulation.add_ctr(lower_level_objective <= objective);

}

idol::Expr<idol::Var, idol::Var>
idol::Bilevel::impl::MinMaxMinFormulation::to_two_stage_robust_formulation_space(const idol::Constant &t_src) const {

    Expr result = t_src.numerical();

    for (const auto& [param, coefficient] : t_src.linear()) {

        const auto& var = param.as<Var>();

        if (m_uncertainty_set.has(var)) {
            result += param * coefficient;
        } else {
            result += coefficient * var;
        }

    }

    return result;

}

void idol::Bilevel::impl::MinMaxMinFormulation::build_extended_lower_level_problem(const std::list<idol::Var>& t_coupling_variables) {

    const auto& high_point_relaxation = m_parent.parent();

    for (const auto& var : m_uncertainty_set.vars()) {

        const double lb = m_uncertainty_set.get_var_lb(var);
        const double ub = m_uncertainty_set.get_var_ub(var);
        const auto type = m_uncertainty_set.get_var_type(var);

        m_extended_lower_level_problem.add(var, TempVar(lb, ub, type, Column(0)));

    }

    for (const auto& var : m_second_stage_dual.vars()) {

        const double lb = m_second_stage_dual.get_var_lb(var);
        const double ub = m_second_stage_dual.get_var_ub(var);
        const auto type = m_second_stage_dual.get_var_type(var);

        m_extended_lower_level_problem.add(var, TempVar(lb, ub, type, Column(0)));

    }

    for (const auto& ctr : m_uncertainty_set.ctrs()) {

        const auto& row = high_point_relaxation.get_ctr_row(ctr);
        const auto type = high_point_relaxation.get_ctr_type(ctr);

        m_extended_lower_level_problem.add(ctr, TempCtr(Row(row), type));

    }

    for (const auto& ctr : m_second_stage_dual.ctrs()) {

        const auto& row = high_point_relaxation.get_ctr_row(ctr);
        const auto type = high_point_relaxation.get_ctr_type(ctr);

        m_extended_lower_level_problem.add(ctr, TempCtr(Row(row), type));

    }

    const auto& lower_level_objective_row = high_point_relaxation.get_ctr_row(m_parent.lower_level_objective());

    assert(lower_level_objective_row.quadratic().empty());

    m_extended_lower_level_problem.set_obj_expr(
            lower_level_objective_row.linear()
            + idol_Sum(var, t_coupling_variables, m_penalty_parameter * (!var + var - 2 * !var * var))
            );

}

const idol::Model &idol::Bilevel::impl::MinMaxMinFormulation::high_point_relaxation() const {
    return m_parent.parent();
}

