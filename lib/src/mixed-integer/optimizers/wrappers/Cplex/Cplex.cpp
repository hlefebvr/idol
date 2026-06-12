//
// Created by Henri on 12/06/2026.
//
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.h"
#include "idol/general/utils/exceptions/Exception.h"

idol::Optimizer *idol::Cplex::create(const Model &t_model) const {

    auto* result = new Optimizers::Cplex(t_model, m_continuous_relaxation.value_or(false));

    if (m_max_n_solution_in_pool.has_value()) {
        result->set_max_n_solution_in_pool(m_max_n_solution_in_pool.value());
    }

    if (!m_logs.value_or(false)) {
        result->set_param_logs(false);
    }

    for (const auto& [param, value] : m_int_params) {
        result->get_dynamic_lib().CPXsetintparam(result->env(), param, value);
    }

    for (const auto& [param, value] : m_double_params) {
        result->get_dynamic_lib().CPXsetdblparam(result->env(), param, value);
    }

    return result;
}

idol::Cplex idol::Cplex::ContinuousRelaxation() {
    return Cplex().with_continuous_relaxation_only(true);
}

idol::Cplex *idol::Cplex::clone() const {
    return new Cplex(*this);
}

idol::Cplex::Cplex(const Cplex& t_src) :
        OptimizerFactoryWithDefaultParameters<Cplex>(t_src),
        m_continuous_relaxation(t_src.m_continuous_relaxation),
        m_max_n_solution_in_pool(t_src.m_max_n_solution_in_pool),
        m_int_params(t_src.m_int_params),
        m_double_params(t_src.m_double_params) {
}

idol::Cplex &idol::Cplex::with_max_n_solution_in_pool(unsigned int t_value) {

    if (m_max_n_solution_in_pool.has_value()) {
        throw Exception("Maximum number of solutions in pool has already been configured.");
    }

    m_max_n_solution_in_pool = t_value;
    return *this;
}

idol::Cplex &idol::Cplex::with_continuous_relaxation_only(bool t_value) {

    if (m_continuous_relaxation.has_value()) {
        throw Exception("Continuous relaxation mode activation has already been configured.");
    }

    m_continuous_relaxation = t_value;
    return *this;
}

idol::Cplex &idol::Cplex::with_external_param(int t_param, int t_value) {

    auto [it, success] = m_int_params.emplace(t_param, t_value);
    if (!success) {
        throw Exception("CPLEX parameter " + std::to_string(t_param) + " has already been configured.");
    }
    return *this;
}

idol::Cplex &idol::Cplex::with_external_param(int t_param, double t_value) {

    auto [it, success] = m_double_params.emplace(t_param, t_value);
    if (!success) {
        throw Exception("CPLEX parameter " + std::to_string(t_param) + " has already been configured.");
    }
    return *this;
}