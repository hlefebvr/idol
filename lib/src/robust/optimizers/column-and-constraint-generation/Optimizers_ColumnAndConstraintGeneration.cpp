//
// Created by henri on 11.12.24.
//
#include <idol/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h>
#include <idol/mixed-integer/modeling/expressions/operations/operators.h>

idol::Optimizers::Robust::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(const idol::Model &t_parent,
                                                                                       const idol::Robust::Description &t_robust_description,
                                                                                       const idol::Bilevel::Description &t_bilevel_description,
                                                                                       const idol::OptimizerFactory &t_master_optimizer,
                                                                                       std::vector<Point<Var>> t_initial_scenarios,
                                                                                       OptimizerFactory* t_initial_scenario_by_minimization,
                                                                                       OptimizerFactory* t_initial_scenario_by_maximization)
                                                                                       : Algorithm(t_parent),
                                                                                         m_robust_description(t_robust_description),
                                                                                         m_bilevel_description(t_bilevel_description),
                                                                                         m_master_optimizer(t_master_optimizer.clone()),
                                                                                         m_initial_scenario_by_minimization(t_initial_scenario_by_minimization),
                                                                                         m_initial_scenario_by_maximization(t_initial_scenario_by_maximization),
                                                                                         m_initial_scenarios(std::move(t_initial_scenarios)) {

}

std::string idol::Optimizers::Robust::ColumnAndConstraintGeneration::name() const {
    return "column-and-constraint generation";
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_var_primal(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_primal");
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_var_reduced_cost(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_reduced_cost");
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_var_ray(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_ray");
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented get_ctr_dual");
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented get_ctr_farkas");
}

unsigned int idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_n_solutions() const {
    throw Exception("Not implemented get_n_solutions");
}

unsigned int idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_solution_index() const {
    throw Exception("Not implemented get_solution_index");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::add(const idol::Var &t_var) {
    throw Exception("Not implemented add");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::add(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented add");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::add(const idol::QCtr &t_ctr) {
    throw Exception("Not implemented add");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::remove(const idol::Var &t_var) {
    throw Exception("Not implemented remove");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::remove(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented remove");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::remove(const idol::QCtr &t_ctr) {
    throw Exception("Not implemented remove");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update() {

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::write(const std::string &t_name) {
    throw Exception("Not implemented write");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::hook_before_optimize() {
    Optimizer::hook_before_optimize();

    m_formulation = std::make_unique<CCG::Formulation>(parent(),
                                                       m_robust_description,
                                                       m_bilevel_description);
    m_formulation->master().use(*m_master_optimizer);

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::hook_optimize() {

    add_initial_scenarios();

    while (true) {

        solve_master_problem();

        if (is_terminated()) {
            break;
        }

        log_iteration();

        ++m_n_iterations;
    }

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::set_solution_index(unsigned int t_index) {
    throw Exception("Not implemented set_solution_index");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_obj_sense() {
    throw Exception("Not implemented update_obj_sense");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_obj() {
    throw Exception("Not implemented update_obj");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_rhs() {
    throw Exception("Not implemented update_rhs");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_obj_constant() {
    throw Exception("Not implemented update_obj_constant");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_mat_coeff(const idol::Ctr &t_ctr,
                                                                               const idol::Var &t_var) {
    throw Exception("Not implemented update_mat_coeff");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_ctr_type(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented update_ctr_type");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_ctr_rhs(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented update_ctr_rhs");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_var_type(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_type");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_var_lb(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_lb");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_var_ub(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_ub");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_var_obj(const idol::Var &t_var) {
    throw Exception("Not implemented update_var_obj");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::add_initial_scenarios() {

    for (const auto& scenario : m_initial_scenarios) {
        m_formulation->add_scenario_to_master(scenario);
    }

    if (m_initial_scenario_by_minimization) {
        add_initial_scenario_by_min_or_max(*m_initial_scenario_by_minimization, 1.);
    }

    if (m_initial_scenario_by_maximization) {
        add_initial_scenario_by_min_or_max(*m_initial_scenario_by_maximization, -1.);
    }

    if (get_param_logs()) {
        std::cout << "Initial scenario pool contains " << m_formulation->n_added_scenarios() << " scenarios." << std::endl;
    }

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::add_initial_scenario_by_min_or_max(const OptimizerFactory& t_optimizer, double t_coefficient) {

    Model model = m_robust_description.uncertainty_set().copy();
    model.set_obj_expr(idol_Sum(var, model.vars(), t_coefficient * var));
    model.use(t_optimizer);
    const double time_limit = std::min<double>(model.optimizer().get_remaining_time(), get_remaining_time());
    model.optimizer().set_param_time_limit(time_limit);
    model.optimize();

    const auto status = model.get_status();
    if (status != Optimal && status != Feasible) {
        throw Exception("Initial scenario by minimization failed.");
    }

    m_formulation->add_scenario_to_master(save_primal(model));

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_master_problem() {

    auto& master = m_formulation->master();
    master.optimizer().set_param_time_limit(get_remaining_time());
    master.optimize();

    const auto status = master.get_status();

    if (status != Optimal) {
        set_status(status);
        set_reason(master.get_reason());
        terminate();
        return;
    }

    set_best_bound(master.get_best_bound());

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::check_termination_criteria() {

    if (get_remaining_time() == 0) {
        set_reason(TimeLimit);
        terminate();
        return;
    }

    if (m_n_iterations > get_param_iteration_limit()) {
        set_reason(IterLimit);
        terminate();
        return;
    }

    if (Algorithm::get_relative_gap() <= get_tol_mip_relative_gap() || Algorithm::get_absolute_gap() <= get_tol_mip_absolute_gap()) {
        set_status(Optimal);
        set_reason(Proved);
        terminate();
        return;
    }

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::log_iteration() {

    if (!get_param_logs()) {
        return;
    }

    std::cout << time().count() << "s\t"
              << m_n_iterations << "\t"
              << get_best_bound() << "\t"
              << get_best_obj() << "\t"
              << get_relative_gap() << "\t"
              << get_absolute_gap() << std::endl;

}
