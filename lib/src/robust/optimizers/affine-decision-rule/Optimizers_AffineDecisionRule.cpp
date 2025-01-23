//
// Created by henri on 28.11.24.
//
#include "idol/robust/optimizers/affine-decision-rule/Optimizers_AffineDecisionRule.h"
#include "idol/robust/optimizers/deterministic/Deterministic.h"

idol::Optimizers::Robust::AffineDecisionRule::AffineDecisionRule(const Model& t_parent,
                                                       const idol::Robust::Description &t_description,
                                                       const OptimizerFactory &t_deterministic_optimizer)
        : Algorithm(t_parent),
          m_description(t_description),
          m_deterministic_optimizer(t_deterministic_optimizer.clone()) {

}

std::string idol::Optimizers::Robust::AffineDecisionRule::name() const {
    return "deterministic";
}

double idol::Optimizers::Robust::AffineDecisionRule::get_var_primal(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    return m_adr_result->model.get_var_primal(t_var);
}

double idol::Optimizers::Robust::AffineDecisionRule::get_var_reduced_cost(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    return m_adr_result->model.get_var_reduced_cost(t_var);
}

double idol::Optimizers::Robust::AffineDecisionRule::get_var_ray(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    return m_adr_result->model.get_var_ray(t_var);
}

double idol::Optimizers::Robust::AffineDecisionRule::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw_if_no_deterministic_model();
    return m_adr_result->model.get_ctr_dual(t_ctr);
}

double idol::Optimizers::Robust::AffineDecisionRule::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw_if_no_deterministic_model();
    return m_adr_result->model.get_ctr_farkas(t_ctr);
}

unsigned int idol::Optimizers::Robust::AffineDecisionRule::get_n_solutions() const {
    throw_if_no_deterministic_model();
    return m_adr_result->model.get_n_solutions();
}

unsigned int idol::Optimizers::Robust::AffineDecisionRule::get_solution_index() const {
    throw_if_no_deterministic_model();
    return m_adr_result->model.get_solution_index();
}

void idol::Optimizers::Robust::AffineDecisionRule::add(const idol::Var &t_var) {
    m_adr_result.reset();
}

void idol::Optimizers::Robust::AffineDecisionRule::add(const idol::Ctr &t_ctr) {
    m_adr_result.reset();
}

void idol::Optimizers::Robust::AffineDecisionRule::add(const idol::QCtr &t_ctr) {
    m_adr_result.reset();
}

void idol::Optimizers::Robust::AffineDecisionRule::remove(const idol::Var &t_var) {
    m_adr_result.reset();
}

void idol::Optimizers::Robust::AffineDecisionRule::remove(const idol::Ctr &t_ctr) {
    m_adr_result.reset();
}

void idol::Optimizers::Robust::AffineDecisionRule::remove(const idol::QCtr &t_ctr) {
    m_adr_result.reset();
}

void idol::Optimizers::Robust::AffineDecisionRule::update() {
    m_adr_result.reset();
}

void idol::Optimizers::Robust::AffineDecisionRule::write(const std::string &t_name) {
    throw_if_no_deterministic_model();
    m_adr_result->model.write(t_name);
}

void idol::Optimizers::Robust::AffineDecisionRule::hook_optimize() {

    if (!m_adr_result) {

        m_adr_result = std::make_unique<idol::Robust::AffineDecisionRule::Result>(
                idol::Robust::AffineDecisionRule::make_model(parent(), m_description)
        );

        m_adr_result->model.use(
                idol::Robust::Deterministic(m_adr_result->description)
                    .with_deterministic_optimizer(*m_deterministic_optimizer)
        );

    }

    m_adr_result->model.optimize();

}

void idol::Optimizers::Robust::AffineDecisionRule::set_solution_index(unsigned int t_index) {
    throw_if_no_deterministic_model();
    m_adr_result->model.set_solution_index(t_index);
}

void idol::Optimizers::Robust::AffineDecisionRule::update_obj_sense() {
    m_adr_result.reset();
}

void idol::Optimizers::Robust::AffineDecisionRule::update_obj() {
    m_adr_result.reset();
}

void idol::Optimizers::Robust::AffineDecisionRule::update_rhs() {
    m_adr_result.reset();
}

void idol::Optimizers::Robust::AffineDecisionRule::update_obj_constant() {
    throw_if_no_deterministic_model();
    m_adr_result->model.set_obj_const(parent().get_obj_expr().affine().constant());
}

void idol::Optimizers::Robust::AffineDecisionRule::update_mat_coeff(const idol::Ctr &t_ctr, const idol::Var &t_var) {
    m_adr_result.reset();
}

void idol::Optimizers::Robust::AffineDecisionRule::update_ctr_type(const idol::Ctr &t_ctr) {
    m_adr_result.reset();
}

void idol::Optimizers::Robust::AffineDecisionRule::update_ctr_rhs(const idol::Ctr &t_ctr) {
    throw_if_no_deterministic_model();
    m_adr_result->model.set_ctr_rhs(t_ctr, parent().get_ctr_rhs(t_ctr));
}

void idol::Optimizers::Robust::AffineDecisionRule::update_var_type(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_adr_result->model.set_var_type(t_var, parent().get_var_type(t_var));
}

void idol::Optimizers::Robust::AffineDecisionRule::update_var_lb(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_adr_result->model.set_var_lb(t_var, parent().get_var_lb(t_var));
}

void idol::Optimizers::Robust::AffineDecisionRule::update_var_ub(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_adr_result->model.set_var_ub(t_var, parent().get_var_ub(t_var));
}

void idol::Optimizers::Robust::AffineDecisionRule::update_var_obj(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_adr_result->model.set_var_obj(t_var, parent().get_var_obj(t_var));
}

void idol::Optimizers::Robust::AffineDecisionRule::throw_if_no_deterministic_model() const {
    if (!m_deterministic_optimizer) {
        throw Exception("Not available.");
    }
}

idol::SolutionStatus idol::Optimizers::Robust::AffineDecisionRule::get_status() const {
    throw_if_no_deterministic_model();
    return m_adr_result->model.get_status();
}

idol::SolutionReason idol::Optimizers::Robust::AffineDecisionRule::get_reason() const {
    throw_if_no_deterministic_model();
    return m_adr_result->model.get_reason();
}

double idol::Optimizers::Robust::AffineDecisionRule::get_best_obj() const {
    throw_if_no_deterministic_model();
    return m_adr_result->model.get_best_obj();
}

double idol::Optimizers::Robust::AffineDecisionRule::get_best_bound() const {
    throw_if_no_deterministic_model();
    return m_adr_result->model.get_best_bound();
}
