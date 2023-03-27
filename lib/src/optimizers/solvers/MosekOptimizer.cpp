//
// Created by henri on 27/03/23.
//
#include "optimizers/solvers/MosekOptimizer.h"
#include "optimizers/solvers/Mosek.h"

Backend *MosekOptimizer::operator()(const AbstractModel &t_model) const {
#ifdef IDOL_USE_MOSEK
    return new Backends::Mosek(t_model, m_continuous_relaxation);
#else
    throw Exception("Idol was not linked with Mosek.");
#endif
}

MosekOptimizer MosekOptimizer::ContinuousRelaxation() {
    return MosekOptimizer(true);
}

MosekOptimizer *MosekOptimizer::clone() const {
    return new MosekOptimizer(*this);
}

