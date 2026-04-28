//
// Created by Henri on 02/04/2026.
//
#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

idol::Optimizer* idol::OptimizerFactory::operator()(const Model& t_model) const {

    auto* result = create(t_model);

    const auto& env = t_model.env();

    // Set parameters
    result->set_param_logs(m_logs.value_or(env.get_param_logs()));
    result->set_param_time_limit(m_time_limit.value_or(env.get_param_time_limit()));
    result->set_param_threads(m_thread_limit.value_or(env.get_param_thread_limit()));
    result->set_param_best_bound_stop(m_best_bound_stop.value_or(env.get_param_best_bound_stop()));
    result->set_param_best_obj_stop(m_best_obj_stop.value_or(env.get_param_best_obj_stop()));
    result->set_param_iteration_limit(m_iteration_count_limit.value_or(env.get_param_iteration_limit()));
    result->set_param_presolve(m_presolve.value_or(env.get_param_presolve()));
    result->set_param_infeasible_or_unbounded_info(m_infeasible_or_unbounded_info.value_or(env.get_param_infeasible_or_unbounded_info()));

    // Set tolerances
    result->set_tol_mip_relative_gap(m_tol_mip_relative_gap.value_or(env.get_tol_mip_relative_gap()));
    result->set_tol_mip_absolute_gap(m_tol_mip_absolute_gap.value_or(env.get_tol_mip_absolute_gap()));
    result->set_tol_integer(m_tol_integer.value_or(env.get_tol_integer()));
    result->set_tol_feasibility(m_tol_feasibility.value_or(env.get_tol_feasibility()));
    result->set_tol_optimality(m_tol_optimality.value_or(env.get_tol_optimality()));

    return result;
}
