//
// Created by henri on 27/03/23.
//
#include "optimizers/solvers/GLPK.h"
#include "optimizers/solvers/Optimizers_GLPK.h"
#include "errors/Exception.h"

Optimizer *GLPK::operator()(const Model &t_model) const {
#ifdef IDOL_USE_GLPK
    auto* result = new Optimizers::GLPK(t_model, m_continuous_relaxation);
    this->handle_default_parameters(result);
    return result;
#else
    throw Exception("idol was not linked with GLPK.");
#endif
}

GLPK GLPK::ContinuousRelaxation() {
    return GLPK(true);
}

GLPK *GLPK::clone() const {
    return new GLPK(*this);
}

