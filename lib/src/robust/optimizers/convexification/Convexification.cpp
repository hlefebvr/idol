//
// Created by Henri on 26/05/2026.
//
#include "idol/robust/optimizers/convexification/Convexification.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/robust/optimizers/convexification/Optimizers_Convexification.h"

idol::Robust::Convexification::Convexification(const idol::Robust::Description& t_robust_description,
                                               const Bilevel::Description &t_bilevel_description) :
    m_robust_description(t_robust_description),
    m_bilevel_description(t_bilevel_description) {

}

idol::Optimizer *idol::Robust::Convexification::create(const idol::Model &t_model) const {

    if (!m_master_problem_optimizer) {
        throw Exception("No optimizer for the master problem has been configured.");
    }

    if (!m_sub_problem_level_optimizer) {
        throw Exception("No optimizer for the sub-problem has been configured.");
    }

    auto* result = new Optimizers::Robust::Convexification(t_model,
                                                m_robust_description,
                                                m_bilevel_description,
                                                *m_master_problem_optimizer,
                                                *m_sub_problem_level_optimizer,
                                                m_integer_master_heuristic_optimizer.get()
                                                );

    return result;
}

idol::OptimizerFactory *idol::Robust::Convexification::clone() const {
    return new Convexification(*this);
}

idol::Robust::Convexification& idol::Robust::Convexification::with_master_optimizer(const OptimizerFactory& t_optimizer) {

    if (m_master_problem_optimizer) {
        throw Exception("An optimizer for solving the master problem has already been configured.");
    }

    m_master_problem_optimizer.reset(t_optimizer.clone());

    return *this;
}

idol::Robust::Convexification& idol::Robust::Convexification::with_sub_problem_optimizer(const OptimizerFactory& t_optimizer) {

    if (m_sub_problem_level_optimizer) {
        throw Exception("An optimizer for the sub-problem has already been configured.");
    }

    m_sub_problem_level_optimizer.reset(t_optimizer.clone());

    return *this;
}

idol::Robust::Convexification& idol::Robust::Convexification::with_integer_master_heuristic_optimizer(
    const OptimizerFactory& t_optimizer) {

    if (m_integer_master_heuristic_optimizer) {
        throw Exception("An optimizer for the integer master heuristic has already been configured.");
    }

    m_integer_master_heuristic_optimizer.reset(t_optimizer.clone());

    return *this;
}

idol::Robust::Convexification::Convexification(const idol::Robust::Convexification &t_src)
        : OptimizerFactoryWithDefaultParameters<Convexification>(t_src),
          m_bilevel_description(t_src.m_bilevel_description),
          m_robust_description(t_src.m_robust_description),
          m_master_problem_optimizer(t_src.m_master_problem_optimizer ? t_src.m_master_problem_optimizer->clone() : nullptr),
          m_sub_problem_level_optimizer(t_src.m_sub_problem_level_optimizer ? t_src.m_sub_problem_level_optimizer->clone() : nullptr),
          m_integer_master_heuristic_optimizer(t_src.m_integer_master_heuristic_optimizer ? t_src.m_integer_master_heuristic_optimizer->clone() : nullptr) {

}
