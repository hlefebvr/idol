//
// Created by henri on 11.04.25.
//
#include "idol/bilevel/optimizers/BranchAndCut/BranchAndCut.h"
#include "idol/bilevel/optimizers/BranchAndCut/Optimizers_BranchAndCut.h"

idol::Bilevel::BranchAndCut::BranchAndCut(const Bilevel::Description &t_description) : m_description(&t_description) {

}

idol::Bilevel::BranchAndCut::BranchAndCut(const idol::Bilevel::BranchAndCut &t_src)
     : m_description(t_src.m_description),
       m_optimizer_for_sub_problems(t_src.m_optimizer_for_sub_problems ? t_src.m_optimizer_for_sub_problems->clone() : nullptr) {

}

void idol::Bilevel::BranchAndCut::set_bilevel_description(const idol::Bilevel::Description &t_bilevel_description) {
    m_description = &t_bilevel_description;
}

idol::Bilevel::BranchAndCut *idol::Bilevel::BranchAndCut::clone() const {
    return new BranchAndCut(*this);
}

idol::Optimizer *idol::Bilevel::BranchAndCut::operator()(const idol::Model &t_model) const {

    if (!m_optimizer_for_sub_problems) {
        throw Exception("No optimizer for sub-problems set.");
    }

    auto* result = new Optimizers::Bilevel::BranchAndCut(t_model, *m_description, *m_optimizer_for_sub_problems);

    handle_default_parameters(result);

    return result;
}

idol::Bilevel::BranchAndCut &
idol::Bilevel::BranchAndCut::with_sub_problem_optimizer(const idol::OptimizerFactory &t_optimizer) {

    if (m_optimizer_for_sub_problems) {
        throw Exception("Sub-problem optimizer already set.");
    }

    m_optimizer_for_sub_problems.reset(t_optimizer.clone());

    return *this;
}

