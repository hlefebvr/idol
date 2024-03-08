//
// Created by henri on 01.02.24.
//
#ifdef IDOL_USE_MIBS

#include "idol/optimizers/bilevel-optimization/wrappers/MibS/Optimizers_MibS.h"

#include <utility>

idol::Optimizers::Bilevel::MibS::MibS(const idol::Model& t_parent,
                             const idol::Annotation<idol::Var, unsigned int> &t_follower_variables,
                             const idol::Annotation<idol::Ctr, unsigned int> &t_follower_constraints,
                             idol::Ctr t_follower_objective)
                             : Optimizer(t_parent),
                               m_follower_variables(t_follower_variables),
                               m_follower_constraints(t_follower_constraints),
                               m_follower_objective(std::move(t_follower_objective)) {

}

idol::SolutionStatus idol::Optimizers::Bilevel::MibS::get_status() const {
    throw_if_no_mibs();
    return m_mibs->get_status();
}

idol::SolutionReason idol::Optimizers::Bilevel::MibS::get_reason() const {
    throw_if_no_mibs();
    return m_mibs->get_reason();
}

double idol::Optimizers::Bilevel::MibS::get_best_obj() const {
    throw_if_no_mibs();
    return m_mibs->get_best_obj();
}

double idol::Optimizers::Bilevel::MibS::get_best_bound() const {
    throw_if_no_mibs();
    return m_mibs->get_best_bound();
}

double idol::Optimizers::Bilevel::MibS::get_var_primal(const idol::Var &t_var) const {
    throw_if_no_mibs();
    return m_mibs->get_var_primal(t_var);
}

double idol::Optimizers::Bilevel::MibS::get_var_ray(const idol::Var &t_var) const {
    throw Exception("Not available.");
}

double idol::Optimizers::Bilevel::MibS::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw Exception("Not available.");
}

double idol::Optimizers::Bilevel::MibS::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw Exception("Not available.");
}

double idol::Optimizers::Bilevel::MibS::get_relative_gap() const {
    throw Exception("Not implemented.");
}

double idol::Optimizers::Bilevel::MibS::get_absolute_gap() const {
    throw Exception("Not implemented.");
}

unsigned int idol::Optimizers::Bilevel::MibS::get_n_solutions() const {
    throw_if_no_mibs();
    const auto status = m_mibs->get_status();
    return status == Optimal || status == Feasible;
}

unsigned int idol::Optimizers::Bilevel::MibS::get_solution_index() const {
    return 1;
}

void idol::Optimizers::Bilevel::MibS::build() {
    m_mibs = std::make_unique<impl::MibS>(parent(),
                                          m_follower_variables,
                                          m_follower_constraints,
                                          m_follower_objective,
                                          get_param_logs());
}

void idol::Optimizers::Bilevel::MibS::add(const idol::Var &t_var) {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::add(const idol::Ctr &t_ctr) {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::remove(const idol::Var &t_var) {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::remove(const idol::Ctr &t_ctr) {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::update() {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::write(const std::string &t_name) {
    throw Exception("Not implemented.");
}

void idol::Optimizers::Bilevel::MibS::hook_optimize() {
    m_mibs.reset();
    build();
    m_mibs->solve();
}

void idol::Optimizers::Bilevel::MibS::set_solution_index(unsigned int t_index) {
    if (t_index == 1 && get_n_solutions() == 1) {
        return;
    }
    throw Exception("Solution index out of bounds.");
}

void idol::Optimizers::Bilevel::MibS::update_obj_sense() {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::update_obj() {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::update_rhs() {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::update_obj_constant() {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::update_mat_coeff(const idol::Ctr &t_ctr, const idol::Var &t_var) {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::update_ctr_type(const idol::Ctr &t_ctr) {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::update_ctr_rhs(const idol::Ctr &t_ctr) {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::update_var_type(const idol::Var &t_var) {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::update_var_lb(const idol::Var &t_var) {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::update_var_ub(const idol::Var &t_var) {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::update_var_obj(const idol::Var &t_var) {
    m_mibs.reset();
}

void idol::Optimizers::Bilevel::MibS::throw_if_no_mibs() const {
    if (m_mibs) {
        return;
    }
    throw Exception("It seems like the model has been modified since your last solve.\n"
                    "Please, call Model::optimize first. ");
}

double idol::Optimizers::Bilevel::MibS::get_var_reduced_cost(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_reduced_cost");
}

#endif
