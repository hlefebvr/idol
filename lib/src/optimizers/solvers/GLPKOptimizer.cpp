//
// Created by henri on 27/03/23.
//
#include "optimizers/solvers/GLPKOptimizer.h"
#include "optimizers/solvers/GLPK.h"
#include "errors/Exception.h"

Backend *GLPKOptimizer::operator()(const AbstractModel &t_model) const {
#ifdef IDOL_USE_GLPK
    return new Backends::GLPK(t_model, m_continuous_relaxation);
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

