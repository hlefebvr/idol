//
// Created by henri on 18.09.24.
//

#include "idol/mixed-integer/optimizers/padm/SubProblem.h"

idol::ADM::SubProblem &idol::ADM::SubProblem::with_optimizer(const idol::OptimizerFactory &t_optimizer_factory) {

    if (m_optimizer_factory) {
        throw Exception("The optimizer has already been set.");
    }

    m_optimizer_factory.reset(t_optimizer_factory.clone());

    return *this;
}

idol::ADM::SubProblem::SubProblem(const idol::ADM::SubProblem & t_src)
        : m_optimizer_factory(t_src.m_optimizer_factory ? t_src.m_optimizer_factory->clone() : nullptr),
          m_initial_point(t_src.m_initial_point) {

}

const idol::OptimizerFactory &idol::ADM::SubProblem::optimizer_factory() const {
    if (!m_optimizer_factory) {
        throw Exception("The optimizer has not been set.");
    }

    return *m_optimizer_factory;
}

idol::ADM::SubProblem &idol::ADM::SubProblem::with_initial_point(const PrimalPoint &t_initial_point) {

    if (m_initial_point) {
        throw Exception("The initial point has already been set.");
    }

    m_initial_point = t_initial_point;

    return *this;
}

idol::PrimalPoint idol::ADM::SubProblem::initial_point() const {
    return m_initial_point.has_value() ? *m_initial_point : PrimalPoint();
}
