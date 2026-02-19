//
// Created by Henri on 03/02/2026.
//
#include "idol/robust/optimizers/nested-branch-and-cut/Formulation.h"
#include "idol/robust/optimizers/nested-branch-and-cut/Optimizers_BilevelBasedBranchAndCut.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

idol::Robust::BBBB::Formulation::Formulation(const idol::Optimizers::Robust::BilevelBasedBranchAndBound& t_parent)
    : m_parent(t_parent),
      m_model(t_parent.parent().copy()),
      m_bilevel_description(t_parent.parent().env()) {

    m_model.unuse();
    build_model();

}


void idol::Robust::BBBB::Formulation::build_model() {

    const auto& robust_optimization = m_parent.get_robust_description();
    const auto& uncertainty_set = robust_optimization.uncertainty_set();
    const auto& bilevel_description = m_parent.get_bilevel_description();
    const auto& original_model = m_parent.parent();

    if (m_parent.relax_first_stage_decisions()) {
        for (const auto& var : original_model.vars()) {
            if (bilevel_description.is_lower(var)) { continue; }
            m_model.set_var_type(var, Continuous);
        }
    }

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

    Map<Pair<Var, Var>, Var> products;
    const auto get_or_create_product_variable = [&](const Var& t_var, const Var& t_unc_param) {

        auto it = products.find({ t_var, t_unc_param });
        if (it != products.end()) {
            return it->second;
        }
        const auto var_type = m_model.get_var_type(t_var);
        const double ub = m_model.get_var_ub(t_var);
        const double lb = m_model.get_var_lb(t_var);
        if (lb <= -Tolerance::Feasibility) {
            throw Exception("Only implemented for non-negative variables.");
        }
        const auto product = m_model.add_var(0., ub, var_type, 0., "__product_" + t_var.name() + "_" + t_unc_param.name());
        // todo One of these is redundant depending on signs...
        const auto c1 = m_model.add_ctr(product <= ub * t_unc_param);
        const auto c2 = m_model.add_ctr(product <= t_var);
        const auto c3 = m_model.add_ctr(product >= t_var - ub * (1 - t_unc_param));
        m_bilevel_description.make_lower_level(product);
        m_bilevel_description.make_lower_level(c1);
        m_bilevel_description.make_lower_level(c2);
        m_bilevel_description.make_lower_level(c3);
        products.emplace(Pair { t_var, t_unc_param }, product);
        return product;
    };

    for (const auto& [ctr, unc_coeffs] : robust_optimization.uncertain_mat_coeffs()) {

        auto lhs = m_model.get_ctr_row(ctr);

        for (const auto& [var, unc_coeff] : unc_coeffs) {
            for (const auto& [unc_param, coeff] : unc_coeff) {
                const auto product = get_or_create_product_variable(var, unc_param);
                lhs += coeff * product;
            }
        }

        m_model.set_ctr_row(ctr, std::move(lhs));

    }

    if (!robust_optimization.uncertain_obj().empty()) {
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
