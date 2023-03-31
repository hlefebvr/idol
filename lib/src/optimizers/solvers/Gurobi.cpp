//
// Created by henri on 27/03/23.
//
#include "optimizers/solvers/Gurobi.h"
#include "optimizers/solvers/Optimizers_Gurobi.h"
#include "errors/Exception.h"

Optimizer *Gurobi::operator()(const Model &t_model) const {
#ifdef IDOL_USE_GUROBI

    auto* result = new Optimizers::Gurobi(t_model, m_continuous_relaxation);

    this->handle_default_parameters(result);

    for (auto* cb : m_callbacks) {
        result->add_callback(cb);
    }

    return result;
#else
    throw Exception("Idol was not linked with Gurobi.");
#endif
}

Gurobi Gurobi::ContinuousRelaxation() {
    return Gurobi(true);
}

Gurobi *Gurobi::clone() const {
    return new Gurobi(*this);
}

Gurobi &Gurobi::with_callback(GurobiCallback *t_cb) {
    m_callbacks.emplace_back(t_cb);
    return *this;
}

