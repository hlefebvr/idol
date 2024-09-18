//
// Created by henri on 18.09.24.
//

#include "AlternatingDirectionMethod.h"
#include "Optimizers_AlternatingDirectionMethod.h"

idol::AlternatingDirectionMethod::AlternatingDirectionMethod(idol::Annotation<idol::Var, unsigned int> t_decomposition)
        : m_decomposition(t_decomposition) {

}

idol::AlternatingDirectionMethod &idol::AlternatingDirectionMethod::with_default_sub_problem_spec(idol::AlternatingDirection::SubProblem t_sub_problem) {

    if (m_default_sub_problem_spec) {
        throw Exception("The default sub-problem has already been set.");
    }

    m_default_sub_problem_spec = std::move(t_sub_problem);

    return *this;

}

idol::Optimizer *idol::AlternatingDirectionMethod::operator()(const idol::Model &t_model) const {

    if (!m_decomposition) {
        throw Exception("The decomposition has not been set.");
    }

    AlternatingDirection::Formulation formulation(t_model,
                                                  *m_decomposition,
                                                  m_penalized_constraints);

    // create sub-problems specs
    auto sub_problem_specs = create_sub_problem_specs(t_model, formulation);

    auto* result = new Optimizers::AlternatingDirectionMethod(
                t_model,
                std::move(formulation),
                std::move(sub_problem_specs)
            );

    handle_default_parameters(result);

    return result;
}

idol::OptimizerFactory *idol::AlternatingDirectionMethod::clone() const {
    return new AlternatingDirectionMethod(*this);
}

idol::AlternatingDirectionMethod &
idol::AlternatingDirectionMethod::with_penalization(const idol::Annotation<idol::Ctr, bool>& t_penalized_constraints) {

    if (m_penalized_constraints) {
        throw Exception("The penalized constraints have already been set.");
    }

    m_penalized_constraints = t_penalized_constraints;

    return *this;
}

std::vector<idol::AlternatingDirection::SubProblem>
idol::AlternatingDirectionMethod::create_sub_problem_specs(const idol::Model &t_model,
                                                           const idol::AlternatingDirection::Formulation &t_formulation) const {

    const unsigned int n_sub_problem = t_formulation.n_sub_problems();

    auto result = std::vector<AlternatingDirection::SubProblem>(n_sub_problem, *m_default_sub_problem_spec);

    for (const auto& [sub_problem_id, sub_problem_spec] : m_sub_problem_specs) {
        result[sub_problem_id] = AlternatingDirection::SubProblem(sub_problem_spec);
    }

    return result;
}
