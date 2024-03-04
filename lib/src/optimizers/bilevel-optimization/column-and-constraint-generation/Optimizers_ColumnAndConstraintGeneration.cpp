//
// Created by henri on 04.03.24.
//

#include "idol/optimizers/bilevel-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"

idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(const idol::Model &t_parent,
                                                                                        const idol::Annotation<idol::Var, unsigned int> &t_lower_level_variables,
                                                                                        const idol::Annotation<idol::Ctr, unsigned int> &t_lower_level_constraints,
                                                                                        idol::Ctr t_lower_level_objective,
                                                                                        const idol::OptimizerFactory &t_master_optimizer,
                                                                                        const idol::OptimizerFactory &t_lower_level_optimizer)
    : idol::Algorithm(t_parent),
      m_lower_level_objective(std::move(t_lower_level_objective)),
      m_lower_level_variables(t_lower_level_variables),
      m_lower_level_constraints(t_lower_level_constraints),
      m_master_optimizer(t_master_optimizer.clone()),
      m_lower_level_optimizer(t_lower_level_optimizer.clone())
                                                                                        {

}

std::string idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::name() const {
    return "bilevel-column-and-constraint-generation";
}

double idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_var_primal(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_primal");
}

double idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_var_reduced_cost(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_reduced_cost");
}

double idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_var_ray(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_ray");
}

double idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented get_ctr_dual");
}

double idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented get_ctr_farkas");
}

unsigned int idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_n_solutions() const {
    throw Exception("Not implemented get_n_solutions");
}

unsigned int idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_solution_index() const {
    throw Exception("Not implemented get_solution_index");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::add(const idol::Var &t_var) {
    throw Exception("Not implemented add");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::add(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented add");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::remove(const idol::Var &t_var) {
    throw Exception("Not implemented remove");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::remove(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented remove");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update() {
    throw Exception("Not implemented update");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::write(const std::string &t_name) {
    throw Exception("Not implemented write");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::hook_optimize() {
    throw Exception("Not implemented hook_optimize");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::set_solution_index(unsigned int t_index) {
    throw Exception("Not implemented set_solution_index");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_obj_sense() {
    throw Exception("Not implemented update_obj_sense");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_obj() {
    throw Exception("Not implemented update_obj");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_rhs() {
    throw Exception("Not implemented update_rhs");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_obj_constant() {
    throw Exception("Not implemented update_obj_constant");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_mat_coeff(const idol::Ctr &t_ctr,
                                                                                const idol::Var &t_var) {
    throw Exception("Not implemented update_mat_coeff");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_ctr_type(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented update_ctr_type");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_ctr_rhs(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented update_ctr_rhs");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_var_type(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_type");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_var_lb(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_lb");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_var_ub(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_ub");
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_var_obj(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_obj");
}
