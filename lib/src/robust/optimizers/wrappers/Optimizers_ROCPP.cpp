//
// Created by Henri on 13/05/2026.
//
#include "idol/robust/optimizers/wrappers/Optimizers_ROCPP.h"
#include "idol/general/optimizers/NestedOptimizer.h"

void idol::Optimizers::Robust::ROCPP::throw_if_no_deterministic_model() const {
    if (!m_model) {
        throw Exception("Not available.");
    }
}

idol::Optimizers::Robust::ROCPP::ROCPP(const Model& t_parent,
                                       const idol::Robust::Description& t_robust_description,
                                       const Bilevel::Description& t_bilevel_description,
                                       idol::Robust::ROCPP::Approximation t_approximation,
                                       const OptimizerFactory& t_optimizer,
                                       unsigned int t_n_policies)  :
    Optimizer(t_parent),
    m_robust_description(t_robust_description),
    m_bilevel_description(t_bilevel_description),
    m_approximation(t_approximation),
    m_optimizer_factory(t_optimizer.clone()),
    m_n_policies(t_n_policies) {

}

idol::SolutionStatus idol::Optimizers::Robust::ROCPP::get_status() const {
    throw_if_no_deterministic_model();
    return m_model->get_status();
}

idol::SolutionReason idol::Optimizers::Robust::ROCPP::get_reason() const {
    throw_if_no_deterministic_model();
    return m_model->get_reason();
}

double idol::Optimizers::Robust::ROCPP::get_best_obj() const {
    throw_if_no_deterministic_model();
    return m_model->get_best_obj();
}

double idol::Optimizers::Robust::ROCPP::get_best_bound() const {
    throw_if_no_deterministic_model();
    return m_model->get_best_bound();
}

double idol::Optimizers::Robust::ROCPP::get_var_primal(const Var& t_var) const {
    throw_if_no_deterministic_model();
    return m_model->get_var_primal(t_var);
}

double idol::Optimizers::Robust::ROCPP::get_var_reduced_cost(const Var& t_var) const {
    throw_if_no_deterministic_model();
    return m_model->get_var_reduced_cost(t_var);
}

double idol::Optimizers::Robust::ROCPP::get_var_ray(const Var& t_var) const {
    throw_if_no_deterministic_model();
    return m_model->get_var_ray(t_var);
}

double idol::Optimizers::Robust::ROCPP::get_ctr_dual(const Ctr& t_ctr) const {
    throw_if_no_deterministic_model();
    return m_model->get_ctr_dual(t_ctr);
}

double idol::Optimizers::Robust::ROCPP::get_ctr_farkas(const Ctr& t_ctr) const {
    throw_if_no_deterministic_model();
    return m_model->get_ctr_farkas(t_ctr);
}

double idol::Optimizers::Robust::ROCPP::get_relative_gap() const {
    throw_if_no_deterministic_model();
    return relative_gap(get_best_bound(), get_best_obj());
}

double idol::Optimizers::Robust::ROCPP::get_absolute_gap() const {
    throw_if_no_deterministic_model();
    return absolute_gap(get_best_bound(), get_best_obj());
}

unsigned idol::Optimizers::Robust::ROCPP::get_n_solutions() const {
    throw_if_no_deterministic_model();
    return m_model->get_n_solutions();
}

unsigned idol::Optimizers::Robust::ROCPP::get_solution_index() const {
    throw_if_no_deterministic_model();
    return m_model->get_solution_index();
}

bool idol::Optimizers::Robust::ROCPP::is_available() {
#ifdef IDOL_USE_ROCPP
    return true;
#else
    return false;
#endif
}

void idol::Optimizers::Robust::ROCPP::hook_optimize() {

    m_model = std::make_unique<Model>(idol::Robust::ROCPP::make_model(parent(), m_robust_description, m_bilevel_description, m_approximation, m_n_policies));
    m_model->use(*m_optimizer_factory);
    
    auto& optimizer = m_model->optimizer();
    inherit_reformulation_parameters(*this, optimizer);
    with_nested_resource_limits(*this, optimizer, [&] { m_model->optimize(); });
}
