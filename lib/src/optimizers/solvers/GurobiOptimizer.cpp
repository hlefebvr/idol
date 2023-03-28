//
// Created by henri on 27/03/23.
//
#include "optimizers/solvers/GurobiOptimizer.h"
#include "optimizers/solvers/Gurobi.h"
#include "errors/Exception.h"

Optimizer *GurobiOptimizer::operator()(const Model &t_model) const {
#ifdef IDOL_USE_GUROBI
    return new Optimizers::Gurobi(t_model, m_continuous_relaxation);
#else
    throw Exception("Idol was not linked with Gurobi.");
#endif
}

GurobiOptimizer GurobiOptimizer::ContinuousRelaxation() {
    return GurobiOptimizer(true);
}

GurobiOptimizer *GurobiOptimizer::clone() const {
    return new GurobiOptimizer(*this);
}

