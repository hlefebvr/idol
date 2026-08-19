//
// Created by charlotte on 05.06.26.
//
#include "idol/general/optimizers/LambdaOptimizer/Optimizers_LambdaOptimizer.h"
#include "idol/general/optimizers/LambdaOptimizer/LambdaContext.h"

idol::Optimizers::LambdaOptimizer::LambdaOptimizer(const Model& t_parent,
                                   const std::function<void(LambdaContext&)>& t_lambda)
        : Algorithm(t_parent),
        m_lambda(t_lambda){
}

std::string idol::Optimizers::LambdaOptimizer::name() const {
    return "LambdaOptimizer";
}

double idol::Optimizers::LambdaOptimizer::get_var_primal(const idol::Var &t_var) const {
    if (m_solution.empty()) {
        throw Exception(std::string(__FUNCTION__) + " solution is empty.");
    }
    const auto& index = parent().get_var_index(t_var);
    return m_solution[index];
}

double idol::Optimizers::LambdaOptimizer::get_var_reduced_cost(const idol::Var &t_var) const {
    throw Exception(std::string(__FUNCTION__) + " not available.");
}

double idol::Optimizers::LambdaOptimizer::get_var_ray(const idol::Var &t_var) const {
    throw Exception(std::string(__FUNCTION__) + " not available.");
}

double idol::Optimizers::LambdaOptimizer::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw Exception(std::string(__FUNCTION__) + " not available.");
}

double idol::Optimizers::LambdaOptimizer::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw Exception(std::string(__FUNCTION__) + " not available.");
}

unsigned int idol::Optimizers::LambdaOptimizer::get_n_solutions() const {
    const auto status = get_status();
    return status == Optimal || status == Feasible;
}

unsigned int idol::Optimizers::LambdaOptimizer::get_solution_index() const {
    return 0;
}

void idol::Optimizers::LambdaOptimizer::add(const idol::Var &t_var) {
    throw Exception(std::string(__FUNCTION__) + " not available.");
}

void idol::Optimizers::LambdaOptimizer::add(const idol::Ctr &t_ctr) {
    throw Exception(std::string(__FUNCTION__) + " not available.");
}

void idol::Optimizers::LambdaOptimizer::add(const idol::QCtr &t_ctr) {
    throw Exception(std::string(__FUNCTION__) + " not available.");
}

void idol::Optimizers::LambdaOptimizer::remove(const idol::Var &t_var) {
    throw Exception(std::string(__FUNCTION__) + " not available.");
}

void idol::Optimizers::LambdaOptimizer::remove(const idol::Ctr &t_ctr) {
    throw Exception(std::string(__FUNCTION__) + " not available.");
}

void idol::Optimizers::LambdaOptimizer::remove(const idol::QCtr &t_ctr) {
    throw Exception(std::string(__FUNCTION__) + " not available.");
}

void idol::Optimizers::LambdaOptimizer::update() {

}

void idol::Optimizers::LambdaOptimizer::write(const std::string &t_name) {
    throw Exception(std::string(__FUNCTION__) + " not available.");
}

void idol::Optimizers::LambdaOptimizer::hook_optimize() {

    if (!m_lambda) {
        throw Exception("No lambda function has been set.");
    }

    LambdaContext ctx(*this);
    m_lambda(ctx);

}

void idol::Optimizers::LambdaOptimizer::set_solution_index(unsigned int t_index){
    if (t_index == 0) {
        return;
    }
    throw Exception("Solution index out of bounds.");
}

void idol::Optimizers::LambdaOptimizer::update_obj() {
    clear_solution();
}

void idol::Optimizers::LambdaOptimizer::update_rhs() {
    clear_solution();
}

void idol::Optimizers::LambdaOptimizer::update_obj_constant() {
    clear_solution();
}

void idol::Optimizers::LambdaOptimizer::update_mat_coeff(const idol::Ctr &t_ctr, const idol::Var &t_var) {
    clear_solution();
}

void idol::Optimizers::LambdaOptimizer::update_ctr_type(const idol::Ctr &t_ctr) {
    clear_solution();
}

void idol::Optimizers::LambdaOptimizer::update_ctr_rhs(const idol::Ctr &t_ctr) {
    clear_solution();
}

void idol::Optimizers::LambdaOptimizer::update_var_type(const idol::Var &t_var) {
    clear_solution();
}

void idol::Optimizers::LambdaOptimizer::update_var_lb(const idol::Var &t_var) {
    clear_solution();
}

void idol::Optimizers::LambdaOptimizer::update_var_ub(const idol::Var &t_var) {
    clear_solution();
}

void idol::Optimizers::LambdaOptimizer::update_var_obj(const idol::Var &t_var) {
    clear_solution();
}

void idol::Optimizers::LambdaOptimizer::clear_solution() {
    m_solution.clear();
    set_status(Loaded);
    set_reason(NotSpecified);
    set_best_bound(-Inf);
    set_best_obj(Inf);
}

void idol::Optimizers::LambdaOptimizer::set_var_primal(const Var& t_var, double t_value) {
    const auto& n_vars = parent().vars().size();
    if (m_solution.size() != n_vars ) {
        m_solution = std::vector(n_vars, 0.);
    }

    const auto &index = parent().get_var_index(t_var);
    m_solution[index] = t_value;
}
