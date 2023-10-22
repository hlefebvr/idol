//
// Created by henri on 27/03/23.
//
#include "idol/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"
#include "idol/errors/Exception.h"

idol::Optimizer *idol::Gurobi::operator()(const Model &t_model) const {
#ifdef IDOL_USE_GUROBI

    auto* result = new Optimizers::Gurobi(t_model, m_continuous_relaxation.has_value() && m_continuous_relaxation.value());

    this->handle_default_parameters(result);

    for (auto& cb : m_callbacks) {
        result->add_callback(cb->operator()());
    }

    if (m_lazy_cuts.has_value()) {
        result->set_lazy_cut(m_lazy_cuts.value());
    }

    if (m_max_n_solution_in_pool.has_value()) {
        result->set_max_n_solution_in_pool(m_max_n_solution_in_pool.value());
    }

    for (const auto [param, value] : m_int_params) {
        result->set_param(param, value);
    }

    for (const auto [param, value] : m_double_params) {
        result->set_param(param, value);
    }

    return result;
#else
    throw Exception("idol was not linked with Gurobi.");
#endif
}

idol::Gurobi idol::Gurobi::ContinuousRelaxation() {
    return Gurobi().with_continuous_relaxation_only(true);
}

idol::Gurobi *idol::Gurobi::clone() const {
    return new Gurobi(*this);
}

idol::Gurobi &idol::Gurobi::with_lazy_cut(bool t_value) {
    m_lazy_cuts = t_value;
    return *this;
}

idol::Gurobi &idol::Gurobi::with_callback(const CallbackFactory &t_cb) {
    m_callbacks.emplace_back(t_cb.clone());
    return *this;
}

idol::Gurobi::Gurobi(const Gurobi& t_src) :
        OptimizerFactoryWithDefaultParameters<Gurobi>(t_src),
        m_continuous_relaxation(t_src.m_continuous_relaxation),
        m_lazy_cuts(t_src.m_lazy_cuts),
        m_double_params(t_src.m_double_params),
        m_int_params(t_src.m_int_params) {

    for (const auto& cb : t_src.m_callbacks) {
        m_callbacks.emplace_back(cb->clone());
    }

}

idol::Gurobi &idol::Gurobi::with_max_n_solution_in_pool(unsigned int t_value) {

    if (m_max_n_solution_in_pool.has_value()) {
        throw Exception("Maximum number of solutions in pool has already been configured.");
    }

    m_max_n_solution_in_pool = t_value;

    return *this;
}

idol::Gurobi &idol::Gurobi::with_continuous_relaxation_only(bool t_value) {

    if (m_continuous_relaxation.has_value()) {
        throw Exception("Continuous relaxation mode activation has already been configured.");
    }

    m_continuous_relaxation = t_value;

    return *this;
}

idol::Gurobi &idol::Gurobi::with_external_param(GRB_IntParam t_param, int t_value) {

#ifdef IDOL_USE_GUROBI
    auto [it, success] = m_int_params.emplace(t_param, t_value);

    if (!success) {
        throw Exception("Gurobi parameter " + std::to_string(t_param) + " has already been configured.");
    }
#endif

    return *this;
}

idol::Gurobi &idol::Gurobi::with_external_param(GRB_DoubleParam t_param, double t_value) {

#ifdef IDOL_USE_GUROBI
    auto [it, success] = m_double_params.emplace(t_param, t_value);

    if (!success) {
        throw Exception("Gurobi parameter " + std::to_string(t_param) + " has already been configured.");
    }
#endif

    return *this;
}
