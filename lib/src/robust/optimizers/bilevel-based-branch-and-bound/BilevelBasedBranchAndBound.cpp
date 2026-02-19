//
// Created by Henri on 03/02/2026.
//
#include <idol/robust/optimizers/nested-branch-and-cut/BilevelBasedBranchAndBound.h>

#include "idol/robust/optimizers/nested-branch-and-cut/Optimizers_BilevelBasedBranchAndCut.h"

idol::Robust::BilevelBasedBranchAndBound::BilevelBasedBranchAndBound(const Robust::Description& t_robust_description, const Bilevel::Description& t_bilevel_description)
    : OptimizerFactoryWithDefaultParameters<idol::Robust::BilevelBasedBranchAndBound>(),
      m_robust_description(t_robust_description),
      m_bilevel_description(t_bilevel_description) {

}

idol::Robust::BilevelBasedBranchAndBound::BilevelBasedBranchAndBound(const BilevelBasedBranchAndBound& t_src)
    : OptimizerFactoryWithDefaultParameters<idol::Robust::BilevelBasedBranchAndBound>(t_src),
      m_robust_description(t_src.m_robust_description),
      m_bilevel_description(t_src.m_bilevel_description),
      m_optimality_bilevel_optimizer(t_src.m_optimality_bilevel_optimizer ? t_src.m_optimality_bilevel_optimizer->clone() : nullptr),
      m_feasibility_bilevel_optimizer(t_src.m_feasibility_bilevel_optimizer ? t_src.m_feasibility_bilevel_optimizer->clone() : nullptr),
      m_with_first_stage_relaxation(t_src.m_with_first_stage_relaxation) {

}

idol::Optimizer* idol::Robust::BilevelBasedBranchAndBound::operator()(const Model& t_model) const {

    if (!m_optimality_bilevel_optimizer) { // TODO build a default optimizer
        throw Exception("No bilevel optimizer for optimality has been configured.");
    }

    auto* result = new Optimizers::Robust::BilevelBasedBranchAndBound(t_model,
                                                              m_robust_description,
                                                              m_bilevel_description,
                                                              *m_optimality_bilevel_optimizer,
                                                              m_feasibility_bilevel_optimizer ? *m_feasibility_bilevel_optimizer : *m_optimality_bilevel_optimizer,
                                                              m_with_first_stage_relaxation.value_or(false)
    );

    handle_default_parameters(result);

    return result;
}

idol::Robust::BilevelBasedBranchAndBound& idol::Robust::BilevelBasedBranchAndBound::with_optimality_bilevel_optimizer(const OptimizerFactory& t_factory) {

    if (m_optimality_bilevel_optimizer) {
        throw Exception("A bilevel optimizer for optimality has already been configure.");
    }

    m_optimality_bilevel_optimizer.reset(t_factory.clone());

    return *this;
}

idol::Robust::BilevelBasedBranchAndBound& idol::Robust::BilevelBasedBranchAndBound::with_feasibility_bilevel_optimizer(const OptimizerFactory& t_factory) {

    if (m_feasibility_bilevel_optimizer) {
        throw Exception("A bilevel optimizer for feasibility has already been configured.");
    }

    m_feasibility_bilevel_optimizer.reset(t_factory.clone());

    return *this;
}

idol::Robust::BilevelBasedBranchAndBound& idol::Robust::BilevelBasedBranchAndBound::with_first_stage_relaxation(bool t_value) {

    if (m_with_first_stage_relaxation) {
        throw Exception("A first-stage relaxation has been configured.");
    }

    m_with_first_stage_relaxation = t_value;

    return *this;
}
