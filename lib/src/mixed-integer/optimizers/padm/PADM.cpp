//
// Created by henri on 18.09.24.
//

#include "idol/mixed-integer/optimizers/padm/PADM.h"

#include <utility>
#include "idol/mixed-integer/optimizers/padm/Optimizers_PADM.h"

idol::PADM::PADM(idol::Annotation<unsigned int> t_decomposition)
        : m_decomposition(std::move(t_decomposition)) {

}

idol::PADM::PADM(idol::Annotation<unsigned int> t_decomposition,
                 idol::Annotation<double> t_penalized_constraints)
        : m_decomposition(std::move(t_decomposition)),
          m_penalized_constraints(t_penalized_constraints) {

}

idol::PADM &idol::PADM::with_default_sub_problem_spec(idol::ADM::SubProblem t_sub_problem) {

    if (m_default_sub_problem_spec) {
        throw Exception("The default sub-problem has already been set.");
    }

    m_default_sub_problem_spec = std::move(t_sub_problem);

    return *this;

}

idol::Optimizer *idol::PADM::operator()(const idol::Model &t_model) const {

    if (!m_penalized_constraints && (m_rescaling || m_penalty_update)) {
        std::cout << "Warning: The penalized constraints have not been set. The rescaling and penalty update will be ignored." << std::endl;
    }

    ADM::Formulation formulation(t_model,
                                 m_decomposition,
                                 m_penalized_constraints,
                                 m_rescaling ? *m_rescaling : -1);

    auto sub_problem_specs = create_sub_problem_specs(t_model, formulation);

    auto* penalty_update = m_penalty_update ? m_penalty_update->clone() : nullptr;
    if (m_penalized_constraints && !penalty_update) {
        penalty_update = new PenaltyUpdates::Additive(1);
    }

    auto* result = new Optimizers::PADM(
                t_model,
                std::move(formulation),
                std::move(sub_problem_specs),
                penalty_update,
                m_feasible_solution_status ? *m_feasible_solution_status : Feasible,
                m_plot_manager ? *m_plot_manager : nullptr
            );

    handle_default_parameters(result);

    return result;
}

idol::OptimizerFactory *idol::PADM::clone() const {
    return new PADM(*this);
}

std::vector<idol::ADM::SubProblem>
idol::PADM::create_sub_problem_specs(const idol::Model &t_model,
                                                           const idol::ADM::Formulation &t_formulation) const {

    const unsigned int n_sub_problem = t_formulation.n_sub_problems();

    std::vector<ADM::SubProblem> result;
    if (m_default_sub_problem_spec.has_value()) {
        result = std::vector<ADM::SubProblem>(n_sub_problem, *m_default_sub_problem_spec);
    } else {
        if (m_sub_problem_specs.size() != n_sub_problem) {
            throw Exception("Not all sub-problems have been specified and no default sub-problem has been given.");
        }
        result = std::vector<ADM::SubProblem>(n_sub_problem, ADM::SubProblem());
    }

    for (const auto& [sub_problem_id, sub_problem_spec] : m_sub_problem_specs) {
        result[sub_problem_id] = ADM::SubProblem(sub_problem_spec);
    }

    return result;
}

idol::PADM &idol::PADM::with_rescaling_threshold(double t_threshold) {

    if (m_rescaling) {
        throw Exception("The rescaling has already been set.");
    }

    m_rescaling = t_threshold;

    return *this;
}

idol::PADM &idol::PADM::with_penalty_update(const idol::PenaltyUpdate &t_penalty_update) {

    if (m_penalty_update) {
        throw Exception("The penalty update has already been set.");
    }

    m_penalty_update.reset(t_penalty_update.clone());

    return *this;
}

idol::PADM::PADM(const idol::PADM &t_src)
    : OptimizerFactoryWithDefaultParameters<PADM>(t_src),
      m_decomposition(t_src.m_decomposition),
      m_penalized_constraints(t_src.m_penalized_constraints),
      m_default_sub_problem_spec(t_src.m_default_sub_problem_spec),
      m_sub_problem_specs(t_src.m_sub_problem_specs),
      m_rescaling(t_src.m_rescaling),
      m_penalty_update(t_src.m_penalty_update ? t_src.m_penalty_update->clone() : nullptr),
      m_feasible_solution_status(t_src.m_feasible_solution_status),
      m_plot_manager(t_src.m_plot_manager) {

}

idol::PADM &idol::PADM::with_feasible_solution_status(idol::SolutionStatus t_status) {

    if (m_feasible_solution_status) {
        throw Exception("The feasible solution status has already been set.");
    }

    m_feasible_solution_status = t_status;

    return *this;
}

idol::PADM &idol::PADM::with_sub_problem_spec(unsigned int t_id, idol::ADM::SubProblem t_sub_problem) {

    auto [it, success] = m_sub_problem_specs.emplace(t_id, std::move(t_sub_problem));

    if (!success) {
        throw Exception("A sub-problem specification with id " + std::to_string(t_id) + " has already been given.");
    }

    return *this;
}

idol::PADM &idol::PADM::with_iteration_plot(Plots::Manager &t_manager) {

    if (m_plot_manager) {
        throw Exception("The plot manager has already been set.");
    }

    m_plot_manager = &t_manager;

    return *this;

}
