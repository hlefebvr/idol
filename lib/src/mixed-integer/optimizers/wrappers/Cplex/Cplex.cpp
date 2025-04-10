//
// Created by henri on 07.04.25.
//
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.h"


idol::Optimizer *idol::Cplex::operator()(const Model &t_model) const {
#ifdef IDOL_USE_CPLEX

    auto* result = new Optimizers::Cplex(t_model, m_continuous_relaxation.has_value() && m_continuous_relaxation.value());

    this->handle_default_parameters(result);

    for (auto& cb : m_callbacks) {
        result->add_callback(cb->operator()());
    }

    if (m_max_n_solution_in_pool.has_value()) {
        result->set_max_n_solution_in_pool(m_max_n_solution_in_pool.value());
    }

    if (m_lazy_cuts.has_value()) {
        result->set_lazy_cuts(m_lazy_cuts.value());
    }

    return result;
#else
    throw Exception("idol was not linked with Cplex.");
#endif
}

idol::Cplex idol::Cplex::ContinuousRelaxation() {
    return Cplex().with_continuous_relaxation_only(true);
}

idol::Cplex *idol::Cplex::clone() const {
    return new Cplex(*this);
}

idol::Cplex &idol::Cplex::add_callback(const CallbackFactory &t_cb) {
    m_callbacks.emplace_back(t_cb.clone());
    return *this;
}

idol::Cplex::Cplex(const Cplex& t_src) :
        OptimizerFactoryWithDefaultParameters<Cplex>(t_src),
        m_continuous_relaxation(t_src.m_continuous_relaxation),
        m_lazy_cuts(t_src.m_lazy_cuts),
        m_max_n_solution_in_pool(t_src.m_max_n_solution_in_pool) {

    for (const auto& cb : t_src.m_callbacks) {
        m_callbacks.emplace_back(cb->clone());
    }

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

idol::Model idol::Cplex::read_from_file(idol::Env &t_env, const std::string &t_filename) {
#if IDOL_USE_CPLEX
    return Optimizers::Cplex::read_from_file(t_env, t_filename);
#else
    throw Exception("idol was not linked with Cplex.");
#endif
}

idol::Cplex &idol::Cplex::with_lazy_cut(bool t_value) {

    if (m_lazy_cuts.has_value()) {
        throw Exception("Lazy cut mode activation has already been configured.");
    }

    m_lazy_cuts = t_value;

    return *this;
}
