//
// Created by henri on 31.10.23.
//
#include "idol/optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.h"

idol::Optimizers::DantzigWolfeDecomposition::DantzigWolfeDecomposition(const Model& t_model,
                                                                       idol::DantzigWolfe::Formulation &&t_formulation,
                                                                       const OptimizerFactory& t_master_optimizer_factory,
                                                                       const DantzigWolfe::DualPriceSmoothingStabilization& t_stabilization,
                                                                       unsigned int t_max_parallel_pricing,
                                                                       bool t_use_hard_branching,
                                                                       std::vector<idol::DantzigWolfe::SubProblem>&& t_sub_problem_specifications,
                                                                       const idol::DantzigWolfe::InfeasibilityStrategyFactory& t_strategy)
    : Algorithm(t_model),
      m_formulation(std::move(t_formulation)),
      m_master_optimizer_factory(t_master_optimizer_factory.clone()),
      m_max_parallel_pricing(t_max_parallel_pricing),
      m_use_hard_branching(t_use_hard_branching),
      m_sub_problem_specifications(std::move(t_sub_problem_specifications)),
      m_stabilization(t_stabilization()),
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
    set_best_bound(m_strategy->best_bound());

}

void idol::Optimizers::DantzigWolfeDecomposition::hook_after_optimize() {
    Optimizer::hook_after_optimize();
}

std::string idol::Optimizers::DantzigWolfeDecomposition::name() const {
    return "dantzig-wolfe";
}

void idol::Optimizers::DantzigWolfeDecomposition::add(const idol::Var &t_var) {
    const auto& parent = this->parent();
    const auto lb = parent.get_var_lb(t_var);
    const auto ub = parent.get_var_ub(t_var);
    const auto type = parent.get_var_type(t_var);
    const auto& column = parent.get_var_column(t_var);
    m_formulation.add(t_var, lb, ub, type, column);
}

void idol::Optimizers::DantzigWolfeDecomposition::add(const idol::Ctr &t_ctr) {
    const auto& parent = this->parent();
    const auto type = parent.get_ctr_type(t_ctr);
    const auto& row = parent.get_ctr_row(t_ctr);
    m_formulation.add(t_ctr, type, row);
}

void idol::Optimizers::DantzigWolfeDecomposition::remove(const idol::Var &t_var) {
    m_formulation.remove(t_var);
}

void idol::Optimizers::DantzigWolfeDecomposition::remove(const idol::Ctr &t_ctr) {
    m_formulation.remove(t_ctr);
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
    return m_formulation.master().get_ctr_dual(t_ctr);
}

double idol::Optimizers::DantzigWolfeDecomposition::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented get_ctr_farkas");
}

unsigned int idol::Optimizers::DantzigWolfeDecomposition::get_n_solutions() const {
    if (const auto status = m_strategy->status() ; status == Feasible || status == Optimal) {
        return 1;
    }
    return 0;
}

unsigned int idol::Optimizers::DantzigWolfeDecomposition::get_solution_index() const {
    if (const auto status = m_strategy->status() ; status == Feasible || status == Optimal) {
        return 1;
    }
    return 0;
}

void idol::Optimizers::DantzigWolfeDecomposition::set_solution_index(unsigned int t_index) {
    if (t_index == 0) {
        return;
    }
    throw Exception("Solution index out of bounds.");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_obj_sense() {
    throw Exception("Not implemented update_obj_sense");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_obj() {
    m_formulation.update_obj(parent().get_obj_expr());
}

void idol::Optimizers::DantzigWolfeDecomposition::update_rhs() {
    throw Exception("Not implemented update_rhs");
}

void idol::Optimizers::DantzigWolfeDecomposition::update_obj_constant() {
    m_formulation.master().set_obj_const(parent().get_obj_expr().constant());
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
    m_formulation.update_var_lb(t_var, parent().get_var_lb(t_var), m_use_hard_branching);
}

void idol::Optimizers::DantzigWolfeDecomposition::update_var_ub(const idol::Var &t_var) {
    m_formulation.update_var_ub(t_var, parent().get_var_ub(t_var), m_use_hard_branching);
}

void idol::Optimizers::DantzigWolfeDecomposition::update_var_obj(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_obj");
}
