//
// Created by henri on 29.11.24.
//
#include "idol/bilevel/optimizers/StrongDuality/Optimizers_StrongDuality.h"
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"


idol::Optimizers::Bilevel::StrongDuality::StrongDuality(const Model& t_parent,
                                                       const idol::Bilevel::Description &t_description,
                                                       const OptimizerFactory &t_deterministic_optimizer)
        : Algorithm(t_parent),
          m_description(t_description),
          m_deterministic_optimizer(t_deterministic_optimizer.clone()) {

}

std::string idol::Optimizers::Bilevel::StrongDuality::name() const {
    return "strong-duality";
}

double idol::Optimizers::Bilevel::StrongDuality::get_var_primal(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_var_primal(t_var);
}

double idol::Optimizers::Bilevel::StrongDuality::get_var_reduced_cost(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_var_reduced_cost(t_var);
}

double idol::Optimizers::Bilevel::StrongDuality::get_var_ray(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_var_ray(t_var);
}

double idol::Optimizers::Bilevel::StrongDuality::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_ctr_dual(t_ctr);
}

double idol::Optimizers::Bilevel::StrongDuality::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_ctr_farkas(t_ctr);
}

unsigned int idol::Optimizers::Bilevel::StrongDuality::get_n_solutions() const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_n_solutions();
}

unsigned int idol::Optimizers::Bilevel::StrongDuality::get_solution_index() const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_solution_index();
}

void idol::Optimizers::Bilevel::StrongDuality::add(const idol::Var &t_var) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Bilevel::StrongDuality::add(const idol::Ctr &t_ctr) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Bilevel::StrongDuality::add(const idol::QCtr &t_ctr) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Bilevel::StrongDuality::remove(const idol::Var &t_var) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Bilevel::StrongDuality::remove(const idol::Ctr &t_ctr) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Bilevel::StrongDuality::remove(const idol::QCtr &t_ctr) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Bilevel::StrongDuality::update() {
    m_deterministic_model.reset();
}

void idol::Optimizers::Bilevel::StrongDuality::write(const std::string &t_name) {
    throw_if_no_deterministic_model();
    m_deterministic_model->write(t_name);
}

void idol::Optimizers::Bilevel::StrongDuality::hook_optimize() {

    if (!m_deterministic_model) {
        m_deterministic_model = std::make_unique<Model>(idol::Bilevel::StrongDuality::make_model(parent(), m_description));
        m_deterministic_model->use(*m_deterministic_optimizer);
    }

    m_deterministic_model->optimize();

}

void idol::Optimizers::Bilevel::StrongDuality::set_solution_index(unsigned int t_index) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_solution_index(t_index);
}

void idol::Optimizers::Bilevel::StrongDuality::update_obj_sense() {
    m_deterministic_model.reset();
}

void idol::Optimizers::Bilevel::StrongDuality::update_obj() {
    m_deterministic_model.reset();
}

void idol::Optimizers::Bilevel::StrongDuality::update_rhs() {
    m_deterministic_model.reset();
}

void idol::Optimizers::Bilevel::StrongDuality::update_obj_constant() {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_obj_const(parent().get_obj_expr().affine().constant());
}

void idol::Optimizers::Bilevel::StrongDuality::update_mat_coeff(const idol::Ctr &t_ctr, const idol::Var &t_var) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Bilevel::StrongDuality::update_ctr_type(const idol::Ctr &t_ctr) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Bilevel::StrongDuality::update_ctr_rhs(const idol::Ctr &t_ctr) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_ctr_rhs(t_ctr, parent().get_ctr_rhs(t_ctr));
}

void idol::Optimizers::Bilevel::StrongDuality::update_var_type(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_type(t_var, parent().get_var_type(t_var));
}

void idol::Optimizers::Bilevel::StrongDuality::update_var_lb(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_lb(t_var, parent().get_var_lb(t_var));
}

void idol::Optimizers::Bilevel::StrongDuality::update_var_ub(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_ub(t_var, parent().get_var_ub(t_var));
}

void idol::Optimizers::Bilevel::StrongDuality::update_var_obj(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_obj(t_var, parent().get_var_obj(t_var));
}

void idol::Optimizers::Bilevel::StrongDuality::throw_if_no_deterministic_model() const {
    if (!m_deterministic_optimizer) {
        throw Exception("Not available.");
    }
}

idol::SolutionStatus idol::Optimizers::Bilevel::StrongDuality::get_status() const {
    if (!m_deterministic_model) {
        return Algorithm::get_status();
    }
    return m_deterministic_model->get_status();
}

idol::SolutionReason idol::Optimizers::Bilevel::StrongDuality::get_reason() const {
    if (!m_deterministic_model) {
        return Algorithm::get_reason();
    }
    return m_deterministic_model->get_reason();
}

double idol::Optimizers::Bilevel::StrongDuality::get_best_obj() const {
    if (!m_deterministic_model) {
        return Algorithm::get_best_obj();
    }
    return m_deterministic_model->get_best_obj();
}

double idol::Optimizers::Bilevel::StrongDuality::get_best_bound() const {
    if (!m_deterministic_model) {
        return Algorithm::get_best_bound();
    }
    return m_deterministic_model->get_best_bound();
}
