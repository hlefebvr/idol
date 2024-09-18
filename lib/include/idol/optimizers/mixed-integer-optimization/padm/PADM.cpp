//
// Created by henri on 18.09.24.
//

#include "PADM.h"

idol::PADM::PADM(idol::Annotation<idol::Var, unsigned int> t_decomposition)
        : m_decomposition(t_decomposition) {

}

idol::PADM::PADM(idol::Annotation<idol::Var, unsigned int> t_decomposition,
                 idol::Annotation<idol::Ctr, bool> t_penalized_constraints)
        : m_decomposition(t_decomposition),
          m_penalized_constraints(t_penalized_constraints) {

}

idol::PADM::SubProblem &idol::PADM::SubProblem::with_optimizer(const idol::OptimizerFactory &t_optimizer_factory) {

    if (m_optimizer_factory) {
        throw Exception("The optimizer has already been set.");
    }

    m_optimizer_factory.reset(t_optimizer_factory.clone());

    return *this;
}

idol::PADM::SubProblem::SubProblem(const idol::PADM::SubProblem & t_src)
    : m_optimizer_factory(t_src.m_optimizer_factory ? t_src.m_optimizer_factory->clone() : nullptr) {

}

idol::PADM &idol::PADM::with_default_sub_problem_spec(idol::PADM::SubProblem t_sub_problem) {

    if (m_default_sub_problem_spec) {
        throw Exception("The default sub-problem has already been set.");
    }

    m_default_sub_problem_spec = std::move(t_sub_problem);

    return *this;

}

idol::Optimizer *idol::PADM::operator()(const idol::Model &t_model) const {
    throw Exception("Not implemented.");
}

idol::OptimizerFactory *idol::PADM::clone() const {
    return new PADM(*this);
}
