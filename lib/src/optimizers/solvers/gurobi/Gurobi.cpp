//
// Created by henri on 27/03/23.
//
#include "optimizers/solvers/gurobi/Gurobi.h"
#include "optimizers/solvers/gurobi/Optimizers_Gurobi.h"
#include "errors/Exception.h"

Optimizer *Gurobi::operator()(const Model &t_model) const {
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

    if (m_use_cuts.has_value()) {
        result->set_use_cuts(m_use_cuts.value());
    }

    if (m_use_heuristics.has_value()) {
        result->set_use_heuristics(m_use_heuristics.value());
    }

    if (m_nonconvexities.has_value()) {
        result->set_nonconvexities(m_nonconvexities.value());
    }

    return result;
#else
    throw Exception("idol was not linked with Gurobi.");
#endif
}

Gurobi Gurobi::ContinuousRelaxation() {
    return Gurobi().with_continuous_relaxation_only(true);
}

Gurobi *Gurobi::clone() const {
    return new Gurobi(*this);
}

Gurobi &Gurobi::with_lazy_cut(bool t_value) {
    m_lazy_cuts = t_value;
    return *this;
}

Gurobi &Gurobi::with_callback(const CallbackFactory &t_cb) {
    m_callbacks.emplace_back(t_cb.clone());
    return *this;
}

Gurobi::Gurobi(const Gurobi& t_src) :
        OptimizerFactoryWithDefaultParameters<Gurobi>(t_src),
        m_continuous_relaxation(t_src.m_continuous_relaxation),
        m_lazy_cuts(t_src.m_lazy_cuts) {

    for (const auto& cb : t_src.m_callbacks) {
        m_callbacks.emplace_back(cb->clone());
    }

}

Gurobi &Gurobi::with_max_n_solution_in_pool(unsigned int t_value) {

    if (m_max_n_solution_in_pool.has_value()) {
        throw Exception("Maximum number of solutions in pool has already been configured.");
    }

    m_max_n_solution_in_pool = t_value;

    return *this;
}

Gurobi &Gurobi::with_cutting_planes(bool t_value) {

    if (m_use_cuts.has_value()) {
        throw Exception("Cut usage has already been configured.");
    }

    m_use_cuts = t_value;

    return *this;
}

Gurobi &Gurobi::with_heuristics(bool t_value) {

    if (m_use_heuristics.has_value()) {
        throw Exception("Heuristic usage has already been configured.");
    }

    m_use_heuristics = t_value;

    return *this;
}

Gurobi &Gurobi::with_continuous_relaxation_only(bool t_value) {

    if (m_continuous_relaxation.has_value()) {
        throw Exception("Continuous relaxation mode activation has already been configured.");
    }

    m_continuous_relaxation = t_value;

    return *this;
}

Gurobi &Gurobi::with_nonconvexities(bool t_value) {

    if (m_nonconvexities.has_value()) {
        throw Exception("Nonconvexities settings has already been configured.");
    }

    m_nonconvexities = t_value;

    return *this;
}
