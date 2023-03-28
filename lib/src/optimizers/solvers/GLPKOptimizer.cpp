//
// Created by henri on 27/03/23.
//
#include "optimizers/solvers/GLPKOptimizer.h"
#include "optimizers/solvers/GLPK.h"
#include "errors/Exception.h"

Optimizer *GLPKOptimizer::operator()(const Model &t_model) const {
#ifdef IDOL_USE_GLPK
    return new Optimizers::GLPK(t_model, m_continuous_relaxation);
#else
    throw Exception("Idol was not linked with GLPK.");
#endif
}

GLPKOptimizer GLPKOptimizer::ContinuousRelaxation() {
    return GLPKOptimizer(true);
}

GLPKOptimizer *GLPKOptimizer::clone() const {
    return new GLPKOptimizer(*this);
}

