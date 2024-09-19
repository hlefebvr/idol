//
// Created by henri on 18.09.24.
//

#include "SubProblem.h"

idol::ADM::SubProblem &idol::ADM::SubProblem::with_optimizer(const idol::OptimizerFactory &t_optimizer_factory) {

    if (m_optimizer_factory) {
        throw Exception("The optimizer has already been set.");
    }

    m_optimizer_factory.reset(t_optimizer_factory.clone());

    return *this;
}

idol::ADM::SubProblem::SubProblem(const idol::ADM::SubProblem & t_src)
        : m_optimizer_factory(t_src.m_optimizer_factory ? t_src.m_optimizer_factory->clone() : nullptr) {

}

const idol::OptimizerFactory &idol::ADM::SubProblem::optimizer_factory() const {
    if (!m_optimizer_factory) {
        throw Exception("The optimizer has not been set.");
    }

    return *m_optimizer_factory;
}
