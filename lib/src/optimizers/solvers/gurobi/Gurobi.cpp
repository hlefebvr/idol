//
// Created by henri on 27/03/23.
//
#include "optimizers/solvers/gurobi/Gurobi.h"
#include "optimizers/solvers/gurobi/Optimizers_Gurobi.h"
#include "errors/Exception.h"

Optimizer *Gurobi::operator()(const Model &t_model) const {
#ifdef IDOL_USE_GUROBI

    auto* result = new Optimizers::Gurobi(t_model, m_continuous_relaxation);

    this->handle_default_parameters(result);

    for (auto* cb : m_callbacks) {
        result->add_callback(cb);
    }

    if (m_lazy_cuts) {
        result->set_lazy_cut(m_lazy_cuts);
    }

    return result;
#else
    throw Exception("idol was not linked with Gurobi.");
#endif
}

Gurobi Gurobi::ContinuousRelaxation() {
    return Gurobi(true);
}

Gurobi *Gurobi::clone() const {
    return new Gurobi(*this);
}

Gurobi &Gurobi::with_callback(Callback *t_cb) {
    m_callbacks.emplace_back(t_cb);
    return *this;
}

Gurobi &Gurobi::with_lazy_cut(bool t_value) {
    m_lazy_cuts = t_value;
    return *this;
}

