//
// Created by henri on 31.10.23.
//
#include "idol/optimizers/column-generation/DantzigWolfeRelaxation.h"
#include "idol/optimizers/column-generation/DantzigWolfeFormulation.h"

idol::OptimizerFactory *idol::DantzigWolfe::Relaxation::clone() const {
    return new Relaxation(*this);
}

idol::DantzigWolfe::Relaxation::Relaxation(idol::Annotation<idol::Ctr, unsigned int> t_decomposition)
    : m_decomposition(std::move(t_decomposition)) {}

idol::DantzigWolfe::Relaxation::Relaxation(const Relaxation& t_src)
    : OptimizerFactoryWithDefaultParameters<Relaxation>(t_src),
      m_decomposition(t_src.m_decomposition),
      m_master_optimizer_factory(t_src.m_master_optimizer_factory->clone())
{}

idol::Optimizer *idol::DantzigWolfe::Relaxation::operator()(const Model &t_model) const {

    auto* dantzig_wolfe_formulation = new Formulation(t_model, m_decomposition);

    add_aggregation_constraints(*dantzig_wolfe_formulation);

    return nullptr;
}

void idol::DantzigWolfe::Relaxation::add_aggregation_constraints(Formulation &t_dantzig_wolfe_formulation) const {

    for (unsigned int i = 0, n = t_dantzig_wolfe_formulation.n_sub_problems() ; i < n ; ++i) {
        const auto& spec = get_sub_problem_spec(i);
        t_dantzig_wolfe_formulation.add_aggregation_constraint(i, spec.lower_multiplicity(), spec.upper_multiplicity());

    }

}

const idol::DantzigWolfe::SubProblem &idol::DantzigWolfe::Relaxation::get_sub_problem_spec(unsigned int t_id) const {

    auto it = m_sub_problem_specs.find(t_id);
    if (it == m_sub_problem_specs.end()) {

        if (!m_default_sub_problem_spec.has_value()) {
            throw Exception("Sub-problem " + std::to_string(t_id) + " has no specification and "
                                      "no default specification has been configured.");
        }

        return m_default_sub_problem_spec.value();
    }

    return it->second;

}

idol::DantzigWolfe::Relaxation &
idol::DantzigWolfe::Relaxation::with_master_optimizer(const idol::OptimizerFactory &t_optimizer_factory) {

    if (m_master_optimizer_factory) {
        throw Exception("An optimizer factory has already been given.");
    }

    m_master_optimizer_factory.reset(t_optimizer_factory.clone());

    return *this;
}

idol::DantzigWolfe::Relaxation &
idol::DantzigWolfe::Relaxation::with_default_sub_problem_spec(idol::DantzigWolfe::SubProblem t_sub_problem) {

    if (m_default_sub_problem_spec) {
        throw Exception("A default sub-problem specification has already been given.");
    }

    m_default_sub_problem_spec.emplace(std::move(t_sub_problem));

    return *this;
}

idol::DantzigWolfe::Relaxation &
idol::DantzigWolfe::Relaxation::with_sub_problem_spec(unsigned int t_id, idol::DantzigWolfe::SubProblem t_sub_problem) {

    auto [it, success] = m_sub_problem_specs.emplace(t_id, std::move(t_sub_problem));

    if (!success) {
        throw Exception("A sub-problem specification with id " + std::to_string(t_id) + " has already been given.");
    }

    return *this;
}

