//
// Created by henri on 27/03/23.
//
#include "optimizers/solvers/Mosek.h"
#include "optimizers/solvers/Optimizers_Mosek.h"
#include "errors/Exception.h"

Optimizer *Mosek::operator()(const Model &t_model) const {
#ifdef IDOL_USE_MOSEK
    auto* result = new Optimizers::Mosek(t_model, m_continuous_relaxation);
    this->handle_default_parameters(result);
    return result;
#else
    throw Exception("Idol was not linked with Mosek.");
#endif
}

Mosek Mosek::ContinuousRelaxation() {
    return Mosek(true);
}

Mosek *Mosek::clone() const {
    return new Mosek(*this);
}

