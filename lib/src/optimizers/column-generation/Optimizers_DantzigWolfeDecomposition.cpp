//
// Created by henri on 31.10.23.
//
#include "idol/optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.h"
#include "idol/optimizers/dantzig-wolfe/column-generation/ColumnGeneration.h"

idol::Optimizers::DantzigWolfeDecomposition::DantzigWolfeDecomposition(const Model& t_model,
                                                                       idol::DantzigWolfe::Formulation &&t_formulation,
                                                                       const OptimizerFactory& t_master_optimizer_factory,
                                                                       unsigned int t_max_parallel_pricing,
                                                                       std::vector<idol::DantzigWolfe::SubProblem>&& t_sub_problem_specifications,
                                                                       const idol::DantzigWolfe::InfeasibilityStrategyFactory& t_strategy)
    : Algorithm(t_model),
      m_formulation(std::move(t_formulation)),
      m_master_optimizer_factory(t_master_optimizer_factory.clone()),
      m_max_parallel_pricing(t_max_parallel_pricing),
      m_sub_problem_specifications(std::move(t_sub_problem_specifications)),
      m_strategy(t_strategy())
{

}

void idol::Optimizers::DantzigWolfeDecomposition::hook_before_optimize() {

    Optimizer::hook_before_optimize();

    set_status(Loaded);
    set_reason(NotSpecified);
    set_best_bound(-Inf);
    set_best_obj(+Inf);

    auto& master = m_formulation.master();

    if (!master.has_optimizer()) {
        master.use(*m_master_optimizer_factory);
    }

}

void idol::Optimizers::DantzigWolfeDecomposition::hook_optimize() {

    m_strategy->execute(*this);

    set_status(m_strategy->status());
    set_reason(m_strategy->reason());
    set_best_obj(m_strategy->best_obj());
    set_best_obj(m_strategy->best_bound());

}

void idol::Optimizers::DantzigWolfeDecomposition::hook_after_optimize() {
    Optimizer::hook_after_optimize();
}

std::string idol::Optimizers::DantzigWolfeDecomposition::name() const {
    return "dantzig-wolfe";
}

void idol::Optimizers::DantzigWolfeDecomposition::add(const idol::Var &t_var) {
    throw Exception("Not implemented add");
}

void idol::Optimizers::DantzigWolfeDecomposition::add(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented add");
}

void idol::Optimizers::DantzigWolfeDecomposition::remove(const idol::Var &t_var) {
    throw Exception("Not implemented remove");
}

void idol::Optimizers::DantzigWolfeDecomposition::remove(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented remove");
}

void idol::Optimizers::DantzigWolfeDecomposition::update() {

    if (m_formulation.master().has_optimizer()) {
        m_formulation.master().update();
    }

    for (auto& sub_problem : m_formulation.sub_problems()) {
        if( sub_problem.has_optimizer() ) {
            sub_problem.update();
        }
    }

}

void idol::Optimizers::DantzigWolfeDecomposition::write(const std::string &t_name) {
    m_formulation.master().write(t_name);
}

double idol::Optimizers::DantzigWolfeDecomposition::get_var_primal(const idol::Var &t_var) const {
    return m_formulation.get_original_space_var_primal(t_var, m_strategy->primal_solution());
}

double idol::Optimizers::DantzigWolfeDecomposition::get_var_ray(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_ray");
}

double idol::Optimizers::DantzigWolfeDecomposition::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented get_ctr_dual");
}

double idol::Optimizers::DantzigWolfeDecomposition::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented get_ctr_farkas");
}

unsigned int idol::Optimizers::DantzigWolfeDecomposition::get_n_solutions() const {
    throw Exception("Not implemented get_n_solutions");
}

unsigned int idol::Optimizers::DantzigWolfeDecomposition::get_solution_index() const {
    throw Exception("Not implemented get_solution_index");
}

void idol::Optimizers::DantzigWolfeDecomposition::set_solution_index(unsigned int t_index) {
    throw Exception("Not implemented set_solution_index");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_obj_sense() {
    throw Exception("Not implemented update_obj_sense");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_obj() {
    throw Exception("Not implemented update_obj");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_rhs() {
    throw Exception("Not implemented update_rhs");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_obj_constant() {
    throw Exception("Not implemented update_obj_constant");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_mat_coeff(const idol::Ctr &t_ctr, const idol::Var &t_var) {
    throw Exception("Not implemented update_mat_coeff");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_ctr_type(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented update_ctr_type");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_ctr_rhs(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented update_ctr_rhs");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_var_type(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_type");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_var_lb(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_lb");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_var_ub(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_ub");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_var_obj(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_obj");
}
