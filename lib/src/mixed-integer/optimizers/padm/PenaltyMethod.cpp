//
// Created by henri on 19.09.24.
//

#include "idol/mixed-integer/optimizers/padm/PenaltyMethod.h"

#include <utility>

idol::PenaltyMethod::PenaltyMethod(idol::Annotation<double> t_penalized_constraints)
    : m_decomposition(t_penalized_constraints.env(), "decomposition", 0),
      m_initial_penalty_parameters(std::move(t_penalized_constraints)) {

}

idol::PenaltyMethod::PenaltyMethod(const idol::PenaltyMethod &t_src)
    : OptimizerFactoryWithDefaultParameters<PenaltyMethod>(t_src),
      m_decomposition(t_src.m_decomposition),
      m_initial_penalty_parameters(t_src.m_initial_penalty_parameters),
      m_rescaling_threshold(t_src.m_rescaling_threshold),
      m_penalty_update(t_src.m_penalty_update ? t_src.m_penalty_update->clone() : nullptr),
      m_optimizer(t_src.m_optimizer ? t_src.m_optimizer->clone() : nullptr),
      m_feasible_solution_status(t_src.m_feasible_solution_status) {

}

idol::PenaltyMethod &idol::PenaltyMethod::with_optimizer(const idol::OptimizerFactory &t_optimizer_factory) {

    if (m_optimizer) {
        throw Exception("The optimizer has already been set.");
    }

    m_optimizer.reset(t_optimizer_factory.clone());

    return *this;
}

idol::PenaltyMethod &idol::PenaltyMethod::with_rescaling_threshold(double t_threshold) {

    if (m_rescaling_threshold) {
        throw Exception("The rescaling has already been set.");
    }

    m_rescaling_threshold = t_threshold;

    return *this;
}

idol::PenaltyMethod &idol::PenaltyMethod::with_penalty_update(const idol::PenaltyUpdate &t_penalty_update) {

    if (m_penalty_update) {
        throw Exception("The penalty update has already been set.");
    }

    m_penalty_update.reset(t_penalty_update.clone());

    return *this;
}

idol::PenaltyMethod *idol::PenaltyMethod::clone() const {
    return new PenaltyMethod(*this);
}

idol::Optimizers::PADM *idol::PenaltyMethod::operator()(const idol::Model &t_model) const {

    if (!m_optimizer) {
        throw Exception("The optimizer has not been set.");
    }

    ADM::Formulation formulation(t_model,
                                 m_decomposition,
                                 m_initial_penalty_parameters,
                                 m_rescaling_threshold ? *m_rescaling_threshold : -1);

    auto* penalty_update = m_penalty_update ? m_penalty_update->clone() : nullptr;
    if (!penalty_update) {
        penalty_update = new PenaltyUpdates::Multiplicative(2);
    }

    auto* result = new Optimizers::PADM(
            t_model,
            std::move(formulation),
            { ADM::SubProblem().with_optimizer(*m_optimizer) },
            penalty_update,
            m_feasible_solution_status ? *m_feasible_solution_status : Feasible,
            nullptr
    );

    handle_default_parameters(result);

    return result;
}

idol::PenaltyMethod &idol::PenaltyMethod::with_feasible_solution_status(idol::SolutionStatus t_status) {

    if (m_feasible_solution_status) {
        throw Exception("The feasible solution status has already been set.");
    }

    m_feasible_solution_status = t_status;

    return *this;
}

idol::PenaltyMethod &idol::PenaltyMethod::operator+=(const idol::OptimizerFactory &t_optimizer_factory) {
    return with_optimizer(t_optimizer_factory);
}

idol::PenaltyMethod operator+(const idol::PenaltyMethod& t_penalty_method, const idol::OptimizerFactory& t_optimizer_factory) {
    return idol::PenaltyMethod(t_penalty_method) += t_optimizer_factory;
}
