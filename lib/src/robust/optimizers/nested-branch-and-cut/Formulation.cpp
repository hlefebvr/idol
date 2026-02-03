//
// Created by Henri on 03/02/2026.
//
#include "idol/robust/optimizers/nested-branch-and-cut/Formulation.h"

#include <assert.h>

#include "idol/robust/optimizers/nested-branch-and-cut/Optimizers_NestedBranchAndCut.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"

idol::Robust::NBC::Formulation::Formulation(const idol::Optimizers::Robust::NestedBranchAndCut& t_parent)
    : m_parent(t_parent),
      m_model(t_parent.parent().copy()),
      m_bilevel_description(t_parent.parent().env()) {

    m_model.unuse();
    build_model();

}


void idol::Robust::NBC::Formulation::build_model() {

    const auto& robust_optimization = m_parent.get_robust_description();
    const auto& uncertainty_set = robust_optimization.uncertainty_set();
    const auto& bilevel_description = m_parent.get_bilevel_description();
    const auto& original_model = m_parent.parent();

    // Add uncertainty set variables
    for (const auto& var : uncertainty_set.vars()) {
        const auto lb = uncertainty_set.get_var_lb(var);
        const auto ub = uncertainty_set.get_var_ub(var);
        const auto type = uncertainty_set.get_var_type(var);
        if (type != Binary) {
            throw Exception("All uncertainty parameters must be binary.");
        }
        m_model.add(var, TempVar(lb, ub, type, 0., LinExpr<Ctr>()));
    }

    // Add uncertainty set constraints
    for (const auto& ctr : uncertainty_set.ctrs()) {
        const auto& row = uncertainty_set.get_ctr_row(ctr);
        const auto rhs = uncertainty_set.get_ctr_rhs(ctr);
        const auto type = uncertainty_set.get_ctr_type(ctr);
        m_model.add(ctr, TempCtr(LinExpr(row), type, rhs));
    }

    // Make lower level variables
    for (const auto& var : original_model.vars()) {
        m_bilevel_description.make_lower_level(var);
        if (bilevel_description.is_upper(var)) {
            m_first_stage_decisions.emplace_back(var);
        }
    }

    // Make lower level constraints
    for (const auto& ctr : original_model.ctrs()) {
        m_bilevel_description.make_lower_level(ctr);
    }

    if (robust_optimization.uncertain_mat_coeffs().size() > 0) {
        throw Exception("Matrix uncertainty is not yet implemented.");
    }

    if (robust_optimization.uncertain_obj().size() > 0) {
        throw Exception("Objective uncertainty is not yet implemented.");
    }

    for (const auto& [ctr, coeff] : robust_optimization.uncertain_rhs()) {
        auto row = m_model.get_ctr_row(ctr);
        m_model.set_ctr_row(ctr, std::move(row) -= coeff);
    }

    // Set objective functions
    auto objective_function = m_model.get_obj_expr();
    m_bilevel_description.set_lower_level_obj(std::move(objective_function));
    m_model.set_obj_expr(-m_bilevel_description.lower_level_obj());

}
