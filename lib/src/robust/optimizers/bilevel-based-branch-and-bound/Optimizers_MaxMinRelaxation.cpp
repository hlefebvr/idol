//
// Created by Henri on 04/06/2026.
//
#include "idol/robust/optimizers/bilevel-based-branch-and-bound/Optimizers_MaxMinRelaxation.h"
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/CriticalValueColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/Optimizers_CriticalValueColumnAndConstraintGeneration.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

idol::Optimizers::Robust::MaxMinRelaxation::MaxMinRelaxation(const Model& t_model,
                                                             const idol::Robust::Description& t_description,
                                                             const idol::Bilevel::Description& t_bilevel_description,
                                                             const OptimizerFactory& t_master_optimizer,
                                                             const OptimizerFactory& t_deterministic_optimizer,
                                                             bool t_use_indicator) : Algorithm(t_model),
                                                                                     m_description(t_description),
                                                                                     m_bilevel_description(t_bilevel_description),
                                                                                     m_master_optimizer_factory(t_master_optimizer.clone()),
                                                                                     m_deterministic_optimizer_factory(t_deterministic_optimizer.clone()),
                                                                                     m_use_indicator(t_use_indicator)
{}

double idol::Optimizers::Robust::MaxMinRelaxation::get_var_primal(const Var& t_var) const {
    throw_if_no_formulation();
    return m_formulation->model.get_var_primal(t_var);
}

void idol::Optimizers::Robust::MaxMinRelaxation::hook_before_optimize() {
    Algorithm::hook_before_optimize();
}

void idol::Optimizers::Robust::MaxMinRelaxation::hook_optimize() {

    if (!m_formulation) {
        build_model();
    }
    
    auto& reformulation = m_formulation->model;
    
    // Parameters
    reformulation.optimizer().set_param_logs(get_param_logs());
    reformulation.optimizer().set_param_time_limit(get_remaining_time());
    reformulation.optimizer().set_param_threads(get_param_thread_limit());
    reformulation.optimizer().set_param_best_bound_stop(get_param_best_bound_stop());
    reformulation.optimizer().set_param_best_obj_stop(get_param_best_obj_stop());
    reformulation.optimizer().set_param_presolve(get_param_presolve());
    reformulation.optimizer().set_param_infeasible_or_unbounded_info(get_param_infeasible_or_unbounded_info());

    // Tolerances
    reformulation.optimizer().set_tol_feasibility(get_tol_feasibility());
    reformulation.optimizer().set_tol_integer(get_tol_integer());
    reformulation.optimizer().set_tol_mip_absolute_gap(get_tol_mip_absolute_gap());
    reformulation.optimizer().set_tol_mip_relative_gap(get_tol_mip_relative_gap());
    reformulation.optimizer().set_tol_optimality(get_tol_optimality());

    m_formulation->model.optimize();

    set_status(m_formulation->model.get_status());
    set_reason(m_formulation->model.get_reason());
    set_best_bound(-m_formulation->model.get_best_bound());
    set_best_obj(-m_formulation->model.get_best_obj());

    /*
    for (const auto& var : parent().vars()) {
        if (var.name()[0] == 'x') {
            std::cout << var.name() << " = " << get_var_primal(var) << std::endl;
        }
    }
    std::cout << "****************" << std::endl;
    */

}

idol::Optimizers::Robust::MaxMinRelaxation::Formulation::Formulation(Model&& t_hpr, idol::Robust::Description&& t_description)
    : model(std::move(t_hpr)), description(std::move(t_description)) {

}

void idol::Optimizers::Robust::MaxMinRelaxation::update_var_lb(const Var& t_var) {
    throw_if_no_formulation();
    auto& ccg_cv = m_formulation->model.optimizer().as<Optimizers::Robust::CriticalValueColumnAndConstraintGeneration>();
    const double lb = parent().get_var_lb(t_var);
    ccg_cv.set_unc_var_lb(t_var, lb);
}

void idol::Optimizers::Robust::MaxMinRelaxation::update_var_ub(const Var& t_var) {
    throw_if_no_formulation();
    auto& ccg_cv = m_formulation->model.optimizer().as<Optimizers::Robust::CriticalValueColumnAndConstraintGeneration>();
    const double ub = parent().get_var_ub(t_var);
    ccg_cv.set_unc_var_ub(t_var, ub);
}

void idol::Optimizers::Robust::MaxMinRelaxation::build_model() {

    assert(m_description.uncertain_mat_coeffs().size() == 0);

    const auto& original_model = parent();
    const auto& original_uncertainty_set = m_description.uncertainty_set();

    Model hpr = m_description.uncertainty_set().copy();
    hpr.unuse();
    hpr.set_obj_sense(Minimize);

    Model uncertainty_set = original_model.copy();
    uncertainty_set.unuse();
    uncertainty_set.set_obj_expr(0.);
    uncertainty_set.set_obj_sense(Minimize);

    // Add all original uncertain parameters (in case of constraint uncertainty)
    for (const auto& unc_var : original_uncertainty_set.vars()) {
        const double lb = original_uncertainty_set.get_var_lb(unc_var);
        const double ub = original_uncertainty_set.get_var_ub(unc_var);
        const auto type = original_uncertainty_set.get_var_type(unc_var);
        uncertainty_set.add(unc_var, TempVar(lb, ub, type, 0, LinExpr<Ctr>()));
    }

    // Add decision-dependent uncertainty set
    for (const auto& [ctr, unc_coeff] : m_description.uncertain_rhs()) {
        uncertainty_set.set_ctr_row(ctr, uncertainty_set.get_ctr_row(ctr) - unc_coeff);
    }

    // Remove original uncertain parameters if not used
    for (const auto& unc_var : original_uncertainty_set.vars()) {
        if (uncertainty_set.get_var_column(unc_var).empty()) {
            uncertainty_set.remove(unc_var);
        }
    }

    idol::Robust::Description description(uncertainty_set);
    const auto one = hpr.add_var(1, 1, Continuous, -original_model.get_obj_expr().affine().constant(), "__one");
    description.set_uncertain_obj(one, -original_model.get_obj_expr().affine().linear());

    for (const auto& [var, unc_coeff] : m_description.uncertain_obj()) {
        for (const auto& [unc_var, coeff] : unc_coeff) {
            assert(hpr.has(unc_var));
            description.set_uncertain_obj(unc_var, description.uncertain_obj(unc_var) - coeff * var);
        }
    }
    //std::cout << idol::Robust::Description::View(original_model, m_description) << std::endl;
    //std::cout << idol::Robust::Description::View(hpr, description) << std::endl;

    m_formulation = std::make_unique<Formulation>(std::move(hpr), std::move(description));

    auto cvccg = idol::Robust::CriticalValueColumnAndConstraintGeneration(m_formulation->description);
    cvccg.with_master_optimizer(*m_master_optimizer_factory);
    cvccg.with_deterministic_optimizer(*m_deterministic_optimizer_factory);
    cvccg.with_indicator(m_use_indicator);

    m_formulation->model.use(cvccg);

    auto& ccg_cv=  m_formulation->model.optimizer().as<Optimizers::Robust::CriticalValueColumnAndConstraintGeneration>();

    std::list<Var> branching_variables;
    for (const auto& var : original_model.vars()) {
        if (m_bilevel_description.is_upper(var) && original_model.get_var_type(var) != Continuous) {
            branching_variables.emplace_back(var);
        }
    }
    ccg_cv.declare_branching_on_unc_vars(std::move(branching_variables));
}

void idol::Optimizers::Robust::MaxMinRelaxation::throw_if_no_formulation() const {
    if (!m_formulation) {
        throw Exception("Not available.");
    }
}
