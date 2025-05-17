//
// Created by henri on 08.05.25.
//
#include "idol/robust/optimizers/benders/Optimizers_Benders.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

idol::Optimizers::Robust::Benders::Benders(const Model &t_parent,
                                           const ::idol::Robust::Description &t_robust_description,
                                           const ::idol::Bilevel::Description &t_bilevel_description,
                                           std::vector<Point<Var>> t_initial_scenarios,
                                           const std::list<std::unique_ptr<OptimizerFactory>> &t_optimizer_feasibility_separation,
                                           const std::list<std::unique_ptr<OptimizerFactory>> &t_optimizer_optimality_separation,
                                           const std::list<std::unique_ptr<OptimizerFactory>> &t_optimizer_joint_separation)
                                           : Algorithm(t_parent),
                                             m_robust_description(t_robust_description),
                                             m_bilevel_description(t_bilevel_description),
                                             m_initial_scenarios(std::move(t_initial_scenarios))
                                           {

    m_optimizer_feasibility_separation.reserve(t_optimizer_feasibility_separation.size());
    for (const auto& optimizer : t_optimizer_feasibility_separation) {
    m_optimizer_feasibility_separation.emplace_back(optimizer->clone());
    }

    m_optimizer_optimality_separation.reserve(t_optimizer_optimality_separation.size());
    for (const auto& optimizer : t_optimizer_optimality_separation) {
    m_optimizer_optimality_separation.emplace_back(optimizer->clone());
    }

    m_optimizer_joint_separation.reserve(t_optimizer_joint_separation.size());
    for (const auto& optimizer : t_optimizer_joint_separation) {
    m_optimizer_joint_separation.emplace_back(optimizer->clone());
    }

}

std::string idol::Optimizers::Robust::Benders::name() const {
    return "benders";
}

double idol::Optimizers::Robust::Benders::get_var_primal(const idol::Var &t_var) const {
    return m_formulation->master().get_var_primal(t_var);
}

double idol::Optimizers::Robust::Benders::get_var_reduced_cost(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_reduced_cost");
}

double idol::Optimizers::Robust::Benders::get_var_ray(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_ray");
}

double idol::Optimizers::Robust::Benders::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented get_ctr_dual");
}

double idol::Optimizers::Robust::Benders::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented get_ctr_farkas");
}

unsigned int idol::Optimizers::Robust::Benders::get_n_solutions() const {
    throw Exception("Not implemented get_n_solutions");
}

unsigned int idol::Optimizers::Robust::Benders::get_solution_index() const {
    throw Exception("Not implemented get_solution_index");
}

void idol::Optimizers::Robust::Benders::add(const idol::Var &t_var) {
    throw Exception("Not implemented add");
}

void idol::Optimizers::Robust::Benders::add(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented add");
}

void idol::Optimizers::Robust::Benders::add(const idol::QCtr &t_ctr) {
    throw Exception("Not implemented add");
}

void idol::Optimizers::Robust::Benders::remove(const idol::Var &t_var) {
    throw Exception("Not implemented remove");
}

void idol::Optimizers::Robust::Benders::remove(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented remove");
}

void idol::Optimizers::Robust::Benders::remove(const idol::QCtr &t_ctr) {
    throw Exception("Not implemented remove");
}

void idol::Optimizers::Robust::Benders::update() {

}

void idol::Optimizers::Robust::Benders::write(const std::string &t_name) {
    throw Exception("Not implemented write");
}

void idol::Optimizers::Robust::Benders::hook_before_optimize() {
    Optimizer::hook_before_optimize();

    m_formulation = std::make_unique<CCG::Formulation>(parent(),
                                                       m_robust_description,
                                                       m_bilevel_description);

    if (m_formulation->is_wait_and_see_follower()) {
        throw Exception("Benders does not support wait-and-see follower.");
    }

    auto optimizer = ::idol::Gurobi();
    optimizer.add_callback(BendersCallback(*this));

    m_formulation->add_epigraph_to_master();
    m_formulation->master().use(optimizer);

}

void idol::Optimizers::Robust::Benders::hook_optimize() {
    m_formulation->master().optimize();
}

void idol::Optimizers::Robust::Benders::set_solution_index(unsigned int t_index) {
    throw Exception("Not implemented set_solution_index");
}

void idol::Optimizers::Robust::Benders::update_obj_sense() {
    throw Exception("Not implemented update_obj_sense");
}

void idol::Optimizers::Robust::Benders::update_obj() {
    throw Exception("Not implemented update_obj");
}

void idol::Optimizers::Robust::Benders::update_rhs() {
    throw Exception("Not implemented update_rhs");
}

void idol::Optimizers::Robust::Benders::update_obj_constant() {
    throw Exception("Not implemented update_obj_constant");
}

void idol::Optimizers::Robust::Benders::update_mat_coeff(const idol::Ctr &t_ctr, const idol::Var &t_var) {
    throw Exception("Not implemented update_mat_coeff");
}

void idol::Optimizers::Robust::Benders::update_ctr_type(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented update_ctr_type");
}

void idol::Optimizers::Robust::Benders::update_ctr_rhs(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented update_ctr_rhs");
}

void idol::Optimizers::Robust::Benders::update_var_type(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_type");
}

void idol::Optimizers::Robust::Benders::update_var_lb(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_lb");
}

void idol::Optimizers::Robust::Benders::update_var_ub(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_ub");
}

void idol::Optimizers::Robust::Benders::update_var_obj(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_obj");
}

idol::Optimizers::Robust::Benders::BendersCallback::BendersCallback(const idol::Optimizers::Robust::Benders &t_parent)
        : m_parent(t_parent) {

}

idol::Callback *idol::Optimizers::Robust::Benders::BendersCallback::operator()() {
    return new Strategy(m_parent);
}

idol::CallbackFactory *idol::Optimizers::Robust::Benders::BendersCallback::clone() const {
    return new BendersCallback(*this);
}

idol::Optimizers::Robust::Benders::BendersCallback::Strategy::Strategy(
        const idol::Optimizers::Robust::Benders &t_parent) : m_parent(t_parent) {

}

void idol::Optimizers::Robust::Benders::BendersCallback::Strategy::operator()(idol::CallbackEvent t_event) {

    if (t_event != IncumbentSolution) {
        return;
    }

    const auto first_stage_decision = primal_solution();

    std::cout << "Separate " << first_stage_decision << std::endl;

    bool has_separated = false;
    for (const auto& scenario : m_scenarios) {
        std::cout << "Separate scenario " << scenario << std::endl;
        throw Exception("STOP!");
    }

    if (!has_separated) {
        std::cout << "Augment scenario pool" << std::endl;
        throw Exception("STOP!");
    }

}
