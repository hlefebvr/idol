//
// Created by henri on 28.06.24.
//
#include "idol/optimizers/robust-optimization/convexification/Convexification.h"

idol::Robust::Convexification::Convexification(idol::Robust::StageDescription t_stage_description,
                                               const idol::Model &t_uncertainty_set)
                                               : OptimizerFactoryWithDefaultParameters<Convexification>(),
                                                 m_stage_description(std::move(t_stage_description)),
                                                 m_uncertainty_set(t_uncertainty_set)
                                               {

}

idol::Optimizer *idol::Robust::Convexification::operator()(const idol::Model &t_model) const {

    if (!m_dantzig_wolfe_optimizer) {
        throw Exception("No Dantzig-Wolfe optimizer has been configured.");
    }

    /*
    auto *result = new Optimizers::Robust::Convexification(t_model,
                                                           m_stage_description,
                                                           m_uncertainty_set,
                                                           *m_dantzig_wolfe_optimizer);
    */

    // return result;

    throw Exception("Not implemented.");
}

idol::Robust::Convexification *idol::Robust::Convexification::clone() const {
    return new Convexification(*this);
}

idol::Robust::Convexification::Convexification(const idol::Robust::Convexification &t_src)
    : OptimizerFactoryWithDefaultParameters<Convexification>(t_src),
      m_stage_description(t_src.m_stage_description),
      m_uncertainty_set(t_src.m_uncertainty_set)
{

}

idol::Robust::Convexification &
idol::Robust::Convexification::with_optimizer(const DantzigWolfeDecomposition &t_optimizer) {

    if (m_dantzig_wolfe_optimizer) {
        throw Exception("Dantzig-Wolfe optimizer already set.");
    }

    m_dantzig_wolfe_optimizer.reset(t_optimizer.clone());

    return *this;
}
