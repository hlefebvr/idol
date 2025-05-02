//
// Created by henri on 20.02.25.
//
#include "idol/bilevel/optimizers/PessimisticAsOptimistic/Optimizers_PessimisticAsOptimistic.h"
#include "idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h"

idol::Optimizers::Bilevel::PessimisticAsOptimistic::PessimisticAsOptimistic(const Model &t_parent,
                                                                            const ::idol::Bilevel::Description &t_description,
                                                                            const OptimizerFactory &t_optimistic_bilevel_optimizer)
                                                                            : Algorithm(t_parent),
                                                                              m_description(t_description),
                                                                              m_optimistic_bilevel_optimizer(t_optimistic_bilevel_optimizer.clone()) {

}

std::string idol::Optimizers::Bilevel::PessimisticAsOptimistic::name() const {
    return "bo-zeng";
}

double idol::Optimizers::Bilevel::PessimisticAsOptimistic::get_var_primal(const idol::Var &t_var) const {
    return m_optimistic_bilevel_model->get_var_primal(t_var);
}

double idol::Optimizers::Bilevel::PessimisticAsOptimistic::get_var_reduced_cost(const idol::Var &t_var) const {
    return m_optimistic_bilevel_model->get_var_reduced_cost(t_var);
}

double idol::Optimizers::Bilevel::PessimisticAsOptimistic::get_var_ray(const idol::Var &t_var) const {
    return m_optimistic_bilevel_model->get_var_ray(t_var);
}

double idol::Optimizers::Bilevel::PessimisticAsOptimistic::get_ctr_dual(const idol::Ctr &t_ctr) const {
    return m_optimistic_bilevel_model->get_ctr_dual(t_ctr);
}

double idol::Optimizers::Bilevel::PessimisticAsOptimistic::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    return m_optimistic_bilevel_model->get_ctr_farkas(t_ctr);
}

unsigned int idol::Optimizers::Bilevel::PessimisticAsOptimistic::get_n_solutions() const {
    return m_optimistic_bilevel_model->get_n_solutions();
}

unsigned int idol::Optimizers::Bilevel::PessimisticAsOptimistic::get_solution_index() const {
    return m_optimistic_bilevel_model->get_solution_index();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::add(const idol::Var &t_var) {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::add(const idol::Ctr &t_ctr) {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::add(const idol::QCtr &t_ctr) {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::remove(const idol::Var &t_var) {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::remove(const idol::Ctr &t_ctr) {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::remove(const idol::QCtr &t_ctr) {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::update() {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::write(const std::string &t_name) {
    m_optimistic_bilevel_model->write(t_name);
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::hook_optimize() {

    m_optimistic_bilevel_model.reset();
    m_optimistic_bilevel_annotation.reset();

    auto [model, annotation] = ::idol::Bilevel::PessimisticAsOptimistic::make_model(parent(), m_description);

    m_optimistic_bilevel_model.emplace(std::move(model));
    m_optimistic_bilevel_annotation.emplace(std::move(annotation));

    m_optimistic_bilevel_optimizer->as<::idol::Bilevel::OptimizerInterface>().set_bilevel_description(*m_optimistic_bilevel_annotation);
    m_optimistic_bilevel_model->use(*m_optimistic_bilevel_optimizer);
    m_optimistic_bilevel_model->optimize();

}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::set_solution_index(unsigned int t_index) {
    m_optimistic_bilevel_model->set_solution_index(t_index);
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::update_obj_sense() {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::update_obj() {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::update_rhs() {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::update_obj_constant() {
    m_optimistic_bilevel_model.reset();
}

void
idol::Optimizers::Bilevel::PessimisticAsOptimistic::update_mat_coeff(const idol::Ctr &t_ctr, const idol::Var &t_var) {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::update_ctr_type(const idol::Ctr &t_ctr) {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::update_ctr_rhs(const idol::Ctr &t_ctr) {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::update_var_type(const idol::Var &t_var) {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::update_var_lb(const idol::Var &t_var) {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::update_var_ub(const idol::Var &t_var) {
    m_optimistic_bilevel_model.reset();
}

void idol::Optimizers::Bilevel::PessimisticAsOptimistic::update_var_obj(const idol::Var &t_var) {
    m_optimistic_bilevel_model.reset();
}

idol::SolutionStatus idol::Optimizers::Bilevel::PessimisticAsOptimistic::get_status() const {
    return m_optimistic_bilevel_model->get_status();
}

idol::SolutionReason idol::Optimizers::Bilevel::PessimisticAsOptimistic::get_reason() const {
    return m_optimistic_bilevel_model->get_reason();
}

double idol::Optimizers::Bilevel::PessimisticAsOptimistic::get_best_obj() const {
    return m_optimistic_bilevel_model->get_best_obj();
}

double idol::Optimizers::Bilevel::PessimisticAsOptimistic::get_best_bound() const {
    return m_optimistic_bilevel_model->get_best_bound();
}

double idol::Optimizers::Bilevel::PessimisticAsOptimistic::get_relative_gap() const {
    return m_optimistic_bilevel_model->optimizer().get_relative_gap();
}

double idol::Optimizers::Bilevel::PessimisticAsOptimistic::get_absolute_gap() const {
    return m_optimistic_bilevel_model->optimizer().get_absolute_gap();
}

