//
// Created by henri on 12.04.25.
//
#include "idol/robust/optimizers/KAdaptabilityBranchAndBound/ScenarioBasedKAdaptabilityProblemSolver.h"
#include "idol/robust/optimizers/KAdaptabilityBranchAndBound/Optimizers_ScenarioBasedKAdaptabilityProblemSolver.h"

idol::Robust::ScenarioBasedKAdaptabilityProblemSolver::ScenarioBasedKAdaptabilityProblemSolver(const Bilevel::Description& t_bilevel_description,
                                                                                               const Robust::Description& t_robust_description,
                                                                                               unsigned int t_K)
    : m_K(t_K), m_bilevel_description(t_bilevel_description), m_robust_description(t_robust_description) {

}

idol::Robust::ScenarioBasedKAdaptabilityProblemSolver::ScenarioBasedKAdaptabilityProblemSolver(const idol::Robust::ScenarioBasedKAdaptabilityProblemSolver &t_src)
        : OptimizerFactoryWithDefaultParameters<ScenarioBasedKAdaptabilityProblemSolver>(t_src),
          m_K(t_src.m_K),
          m_bilevel_description(t_src.m_bilevel_description),
          m_robust_description(t_src.m_robust_description),
          m_main_optimizer(t_src.m_main_optimizer ? t_src.m_main_optimizer->clone() : nullptr),
          m_separation_optimizer(t_src.m_separation_optimizer ? t_src.m_separation_optimizer->clone() : nullptr) {

}

idol::Optimizer *idol::Robust::ScenarioBasedKAdaptabilityProblemSolver::operator()(const idol::Model &t_model) const {

    if (m_K == 0) {
        throw Exception("K must be at least 1.");
    }

    if (!m_main_optimizer) {
        throw Exception("No optimizer has been set.");
    }

    OptimizerFactory* separation_optimizer = m_separation_optimizer ? m_separation_optimizer.get() : m_main_optimizer.get();

    auto* result = new Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver(t_model,
                                                                                   m_bilevel_description,
                                                                                   m_robust_description,
                                                                                   m_K,
                                                                                   *m_main_optimizer,
                                                                                   *separation_optimizer);

    handle_default_parameters(result);

    return result;
}

idol::Robust::ScenarioBasedKAdaptabilityProblemSolver*
idol::Robust::ScenarioBasedKAdaptabilityProblemSolver::clone() const {
    return new ScenarioBasedKAdaptabilityProblemSolver(*this);
}

idol::Robust::ScenarioBasedKAdaptabilityProblemSolver &
idol::Robust::ScenarioBasedKAdaptabilityProblemSolver::with_optimizer(const idol::OptimizerFactory &t_optimizer) {

    if (m_main_optimizer) {
        throw Exception("An optimizer has already been set.");
    }

    m_main_optimizer.reset(t_optimizer.clone());

    return *this;
}

idol::Robust::ScenarioBasedKAdaptabilityProblemSolver &
idol::Robust::ScenarioBasedKAdaptabilityProblemSolver::with_separation_optimizer(
        const idol::OptimizerFactory &t_optimizer) {

    if (m_separation_optimizer) {
        throw Exception("An optimizer has already been set.");
    }

    m_separation_optimizer.reset(t_optimizer.clone());

    return *this;
}
