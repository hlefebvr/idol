//
// Created by henri on 12.01.26.
//
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation-algorithm/Formulation.h"
#include <idol/robust/optimizers/critical-value-column-and-constraint-generation-algorithm/Optimizers_CriticalValueColumnAndConstraintGeneration.h>
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

idol::CVCCG::Formulation::Formulation(
    const idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration& t_parent) : m_parent(t_parent),
    m_master(t_parent.parent().copy()) {

    initialize_master_problem();
    initialize_separation_problems();
    initialize_critical_values();

}

void idol::CVCCG::Formulation::initialize_master_problem() {
    m_master.unuse();

    const auto& robust_description = m_parent.m_robust_description;
    const auto& uncertainty_set = robust_description.uncertainty_set();

    for (const auto& [ctr, _] : robust_description.uncertain_mat_coeffs()) {
        const auto it = m_separation_problems.find(ctr);
        if (it == m_separation_problems.end()) {
            m_separation_problems.emplace(ctr, uncertainty_set.copy());
        }
        m_master.remove(ctr);
    }

    for (const auto& [ctr, _] : robust_description.uncertain_rhs()) {
        const auto it = m_separation_problems.find(ctr);
        if (it == m_separation_problems.end()) {
            m_separation_problems.emplace(ctr, uncertainty_set.copy());
        }
        m_master.remove(ctr);
    }

    m_master.use(*m_parent.m_master_optimizer);
}

void idol::CVCCG::Formulation::initialize_separation_problems() {

    const auto& model = m_parent.parent();
    const auto& robust_description = m_parent.m_robust_description;
    const auto& uncertainty_set = robust_description.uncertainty_set();

    for (const auto& var : uncertainty_set.vars()) {
        if (!model.has(var)) {
            continue;
        }
        for (auto& [ctr, separation_problem] : m_separation_problems) {
            separation_problem.remove(var);
        }
    }

    auto& optimizer = *m_parent.m_separation_optimizer;
    for (auto& [ctr, uncertainty_set] : m_separation_problems) {
        uncertainty_set.use(optimizer);
    }

}

void idol::CVCCG::Formulation::initialize_critical_values() {

    const auto& parent = m_parent.parent();
    const auto& uncertainty_set = m_parent.m_robust_description.uncertainty_set();

    for (const auto& ctr : uncertainty_set.ctrs()) {

        const auto& row = uncertainty_set.get_ctr_row(ctr);

        bool is_decision_dependent_constraint = false;
        for (const auto& [var, coefficient] : row) {
            if (!parent.has(var)) {
                continue;
            }
            is_decision_dependent_constraint = true;
            break;
        }

        if (!is_decision_dependent_constraint) {
            continue;
        }

        m_critical_values.emplace(ctr, Map<long int, Var>());
    }
}

void idol::CVCCG::Formulation::update_separation_problems(const PrimalPoint& t_master_solution) {

    for (auto& [ctr, separation_problem] : m_separation_problems) {
        update_separation_problem_objective(ctr, separation_problem, t_master_solution);
    }

    update_separation_problems_constraints(t_master_solution);

}

void idol::CVCCG::Formulation::update_separation_problem_objective(const Ctr& t_ctr, Model& t_separation_problem, const PrimalPoint& t_master_solution) {

    const auto& model = m_parent.parent();
    const auto& robust_description = m_parent.m_robust_description;
    const auto& row = model.get_ctr_row(t_ctr);
    const double rhs = model.get_ctr_rhs(t_ctr);
    const auto& uncertain_rhs = robust_description.uncertain_rhs(t_ctr);
    const auto& uncertain_mat = robust_description.uncertain_mat_coeffs(t_ctr);

    // Set objective
    AffExpr<Var, double> objective_function = evaluate(row, t_master_solution);
    objective_function -= rhs + uncertain_rhs;
    for (const auto& [var, expr] : uncertain_mat) {
        objective_function += t_master_solution.get(var) * expr;
    }

    if (model.get_ctr_type(t_ctr) == LessOrEqual) {
        objective_function *= -1;
    }

    t_separation_problem.set_obj_expr(std::move(objective_function));

}

void idol::CVCCG::Formulation::update_separation_problems_constraints(const PrimalPoint& t_master_solution) {

    const auto& robust_description = m_parent.m_robust_description;
    const auto& uncertainty_set = robust_description.uncertainty_set();

    for (const auto& ctr : uncertainty_set.ctrs()) {

        const auto& row = uncertainty_set.get_ctr_row(ctr);
        const double rhs_val = uncertainty_set.get_ctr_rhs(ctr) - evaluate(row, t_master_solution);

        for (auto& [_, separation_problem] : m_separation_problems) {
            separation_problem.set_ctr_rhs(ctr, rhs_val);
        }

    }

}

void idol::CVCCG::Formulation::add_scenario(const Ctr& t_ctr, const PrimalPoint& t_scenario) {

    // TODO: extract this
    const auto filter = [](const LinExpr<Var>& t_expr, const std::function<bool(const Var&)>& t_predicate) {
        LinExpr<Var> result;
        for (const auto& [var, coefficient] : t_expr) {
            if (t_predicate(var)) {
                result += coefficient * var;
            }
        }
        return result;
    };

    // Constraint without indicator
    const auto& model = m_parent.parent();
    const auto& robust_description = m_parent.m_robust_description;
    const auto& uncertainty_set = robust_description.uncertainty_set();
    const double rhs = model.get_ctr_rhs(t_ctr) + evaluate(robust_description.uncertain_rhs(t_ctr), t_scenario);
    LinExpr lhs = model.get_ctr_row(t_ctr);
    for (const auto& [var, coefficient] : robust_description.uncertain_mat_coeffs(t_ctr)) {
        lhs += evaluate(coefficient, t_scenario) * var;
    }

    // Add indicators
    std::vector<Var> indicators;
    indicators.reserve(m_critical_values.size());
    for (auto& [ctr, critical_values] : m_critical_values) {

        // Compute critical value
        const double critical_value = uncertainty_set.get_ctr_rhs(ctr) - evaluate(uncertainty_set.get_ctr_row(ctr), t_scenario) + 1;

        const auto it = critical_values.find(critical_value);
        if (it != critical_values.end()) {
            indicators.emplace_back(it->second);
            continue;
        }

        auto name = std::string("indicator_").append(ctr.name()).append("_").append(std::to_string(m_parent.m_n_iterations));
        const auto indicator = m_master.add_var(0, 1, Binary, 0, std::move(name));
        indicators.emplace_back(indicator);
        critical_values.emplace(std::round(critical_value), indicator);

        const auto& row = uncertainty_set.get_ctr_row(ctr);
        const auto type = uncertainty_set.get_ctr_type(ctr);
        LinExpr decision_dependent_part = filter(row, [&](const Var& t_var){ return model.has(t_var); });
        if (type == LessOrEqual) {
            m_master.add_ctr(critical_value * indicator <= decision_dependent_part);
        } else {
            throw Exception("This case is not implemented yet.");
        }

    }

    const double big_M = 1e5; // TODO compute this exactly
    const auto type = model.get_ctr_type(t_ctr);
    if (type == LessOrEqual) {
        m_master.add_ctr(lhs <= rhs + big_M * idol_Sum(z, indicators, z));
    } else {
        throw Exception("This case is not implemented yet.");
    }

}
