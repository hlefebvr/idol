//
// Created by charlotte on 05.06.26.
//
#include "idol/general/optimizers/LambdaOptimizer/Optimizers_LambdaOptimizer.h"
#include "idol/general/optimizers/LambdaOptimizer/LambdaOptimizer.h"

idol::Optimizers::LambdaOptimizer::LambdaOptimizer(const Model& t_parent,
                                    LambdaContext& t_lambda_context,
                                   const std::function<void(LambdaContext&, const Model&)>& t_lambda)
        : Algorithm(t_parent),
        m_deterministic_model(t_parent.clone()),
        m_lambda_context(t_lambda_context),
        m_lambda(t_lambda){
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl;
    std::cout << m_deterministic_model->get_obj_expr() << std::endl;
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl;  

}

std::string idol::Optimizers::LambdaOptimizer::name() const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl;
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl;  
    return "LambdaOptimizer";
}

double idol::Optimizers::LambdaOptimizer::get_var_primal(const idol::Var &t_var) const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    return m_deterministic_model->get_var_primal(t_var);
}

double idol::Optimizers::LambdaOptimizer::get_var_reduced_cost(const idol::Var &t_var) const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    return m_deterministic_model->get_var_reduced_cost(t_var);
}

double idol::Optimizers::LambdaOptimizer::get_var_ray(const idol::Var &t_var) const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    return m_deterministic_model->get_var_ray(t_var);
}

double idol::Optimizers::LambdaOptimizer::get_ctr_dual(const idol::Ctr &t_ctr) const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    return m_deterministic_model->get_ctr_dual(t_ctr);
}

double idol::Optimizers::LambdaOptimizer::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    return m_deterministic_model->get_ctr_farkas(t_ctr);
}

unsigned int idol::Optimizers::LambdaOptimizer::get_n_solutions() const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    return m_deterministic_model->get_n_solutions();
}

unsigned int idol::Optimizers::LambdaOptimizer::get_solution_index() const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    return m_deterministic_model->get_solution_index();
}

void idol::Optimizers::LambdaOptimizer::add(const idol::Var &t_var) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    m_deterministic_model.reset();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::add(const idol::Ctr &t_ctr) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    m_deterministic_model.reset();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::add(const idol::QCtr &t_ctr) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    m_deterministic_model.reset();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::remove(const idol::Var &t_var) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    m_deterministic_model.reset();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::remove(const idol::Ctr &t_ctr) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    m_deterministic_model.reset();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::remove(const idol::QCtr &t_ctr) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    m_deterministic_model.reset();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::update() {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::write(const std::string &t_name) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    m_deterministic_model->write(t_name);
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::hook_optimize() {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    
    throw_if_no_deterministic_model();

    if (!m_lambda) {
        throw Exception("No lambda function has been set.");
    }

    m_lambda(m_lambda_context, *m_deterministic_model);

    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 

}

void idol::Optimizers::LambdaOptimizer::set_solution_index(unsigned int t_index) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    m_deterministic_model->set_solution_index(t_index);
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::update_obj_sense() {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    m_deterministic_model.reset();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::update_obj() {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    m_deterministic_model.reset();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::update_rhs() {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    m_deterministic_model.reset();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::update_obj_constant() {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    m_deterministic_model->set_obj_const(parent().get_obj_expr().affine().constant());
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::update_mat_coeff(const idol::Ctr &t_ctr, const idol::Var &t_var) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    m_deterministic_model.reset();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::update_ctr_type(const idol::Ctr &t_ctr) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    m_deterministic_model.reset();
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::update_ctr_rhs(const idol::Ctr &t_ctr) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    m_deterministic_model->set_ctr_rhs(t_ctr, parent().get_ctr_rhs(t_ctr));
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::update_var_type(const idol::Var &t_var) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_type(t_var, parent().get_var_type(t_var));
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::update_var_lb(const idol::Var &t_var) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_lb(t_var, parent().get_var_lb(t_var));
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::update_var_ub(const idol::Var &t_var) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_ub(t_var, parent().get_var_ub(t_var));
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::update_var_obj(const idol::Var &t_var) {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_obj(t_var, parent().get_var_obj(t_var));
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

void idol::Optimizers::LambdaOptimizer::throw_if_no_deterministic_model() const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    if (!m_deterministic_model) {
        throw Exception("Not available.");
    }
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

idol::SolutionStatus idol::Optimizers::LambdaOptimizer::get_status() const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    if (!m_deterministic_model) {
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
        return Algorithm::get_status();
    }
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    return m_deterministic_model->get_status();
}

idol::SolutionReason idol::Optimizers::LambdaOptimizer::get_reason() const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    if (!m_deterministic_model) {
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
        return Algorithm::get_reason();
    }
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    return m_deterministic_model->get_reason();
}

double idol::Optimizers::LambdaOptimizer::get_best_obj() const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    if (!m_deterministic_model) {
        return Algorithm::get_best_obj();
    }
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    return m_deterministic_model->get_best_obj();
}

double idol::Optimizers::LambdaOptimizer::get_best_bound() const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    if (!m_deterministic_model) {
        return Algorithm::get_best_bound();
    }
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    return m_deterministic_model->get_best_bound();
}
