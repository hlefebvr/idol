//
// Created by henri on 18.09.24.
//

#include "Optimizers_PADM.h"

idol::Optimizers::PADM::PADM(const Model& t_model,
                             ADM::Formulation t_formulation,
                             std::vector<idol::ADM::SubProblem>&& t_sub_problem_specs)
                            : Algorithm(t_model),
                              m_formulation(std::move(t_formulation)),
                              m_sub_problem_specs(std::move(t_sub_problem_specs)) {

}

double idol::Optimizers::PADM::get_var_primal(const idol::Var &t_var) const {
    return m_formulation.sub_problem(t_var).get_var_primal(t_var);
}

double idol::Optimizers::PADM::get_var_reduced_cost(const idol::Var &t_var) const {
    return m_formulation.sub_problem(t_var).get_var_reduced_cost(t_var);
}

double idol::Optimizers::PADM::get_var_ray(const idol::Var &t_var) const {
    return m_formulation.sub_problem(t_var).get_var_ray(t_var);
}

double idol::Optimizers::PADM::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented");
}

double idol::Optimizers::PADM::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented");
}

unsigned int idol::Optimizers::PADM::get_n_solutions() const {
    return Algorithm::get_status() == Feasible ? 1 : 0;
}

unsigned int idol::Optimizers::PADM::get_solution_index() const {
    return 0;
}

void idol::Optimizers::PADM::add(const idol::Var &t_var) {
    throw Exception("Not implemented");
}

void idol::Optimizers::PADM::add(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented");
}

void idol::Optimizers::PADM::remove(const idol::Var &t_var) {
    throw Exception("Not implemented");
}

void idol::Optimizers::PADM::remove(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented");
}

void idol::Optimizers::PADM::update() {

}

void idol::Optimizers::PADM::write(const std::string &t_name) {
    throw Exception("Not implemented");
}


void idol::Optimizers::PADM::hook_before_optimize() {
    Optimizer::hook_before_optimize();

    set_status(Loaded);
    set_reason(NotSpecified);
    set_best_obj(Inf);
    set_best_bound(-Inf);

    const unsigned int n_sub_problems = m_formulation.n_sub_problems();

    m_outer_loop_iteration = 0;
    m_inner_loop_iterations = 0;
    m_last_solutions = std::vector<Solution::Primal>(n_sub_problems);

    for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
        auto& model = m_formulation.sub_problem(i);
        if (!model.has_optimizer()) {
            model.use(m_sub_problem_specs[i].optimizer_factory());
        }
    }
}


void idol::Optimizers::PADM::hook_optimize() {

    for (unsigned int max_outer_loop = get_param_iteration_limit() ; m_outer_loop_iteration < max_outer_loop ; ++m_outer_loop_iteration) {

        update_penalty_parameters();

        run_inner_loop();

        if (is_feasible()) {
            set_status(Feasible);
            set_reason(Proved);
            break;
        }

    }

}

void idol::Optimizers::PADM::set_solution_index(unsigned int t_index) {
    if (t_index != 0) {
        throw Exception("Invalid solution index");
    }
}

void idol::Optimizers::PADM::update_obj_sense() {
    throw Exception("Not implemented");
}

void idol::Optimizers::PADM::update_obj() {
    throw Exception("Not implemented");
}

void idol::Optimizers::PADM::update_rhs() {
    throw Exception("Not implemented");
}

void idol::Optimizers::PADM::update_obj_constant() {
    throw Exception("Not implemented");
}

void idol::Optimizers::PADM::update_mat_coeff(const idol::Ctr &t_ctr, const idol::Var &t_var) {
    throw Exception("Not implemented");
}

void idol::Optimizers::PADM::update_ctr_type(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented");
}

void idol::Optimizers::PADM::update_ctr_rhs(const idol::Ctr &t_ctr) {
    throw Exception("Not implemented");
}

void idol::Optimizers::PADM::update_var_type(const idol::Var &t_var) {
    const auto type = parent().get_var_type(t_var);
    m_formulation.sub_problem(t_var).set_var_type(t_var, type);
}

void idol::Optimizers::PADM::update_var_lb(const idol::Var &t_var) {
    const auto lb = parent().get_var_lb(t_var);
    m_formulation.sub_problem(t_var).set_var_lb(t_var, lb);
}

void idol::Optimizers::PADM::update_var_ub(const idol::Var &t_var) {
    const auto ub = parent().get_var_ub(t_var);
    m_formulation.sub_problem(t_var).set_var_ub(t_var, ub);
}

void idol::Optimizers::PADM::update_var_obj(const idol::Var &t_var) {
    throw Exception("Not implemented");
}

bool idol::Optimizers::PADM::is_feasible() const {
    for (unsigned int i = 0, n = m_formulation.n_sub_problems() ; i < n ; ++i) {
        if (!is_feasible(i)) {
            return false;
        }
    }
    return true;
}

bool idol::Optimizers::PADM::is_feasible(unsigned int t_sub_problem_id) const {

    for (const auto& var : m_formulation.l1_vars(t_sub_problem_id)) {
        if (m_last_solutions[t_sub_problem_id].get(var) > 1e-4) {
            return false;
        }
    }

    return true;
}

void idol::Optimizers::PADM::run_inner_loop() {

    for (unsigned int inner_loop_iteration = 0 ; inner_loop_iteration < m_max_inner_loop_iterations ; ++inner_loop_iteration) {

        bool has_changed = false;

        for (unsigned int i = 0, n = m_formulation.n_sub_problems() ; i < n ; ++i) {
            has_changed |= solve_sub_problem(i);
        }

        if (!has_changed || is_terminated()) {
            break;
        }

        ++m_inner_loop_iterations;
    }

}

void idol::Optimizers::PADM::update_penalty_parameters() {

    m_formulation.update_penalty_parameters(m_last_solutions);

}

bool idol::Optimizers::PADM::solve_sub_problem(unsigned int t_sub_problem_id) {

    m_formulation.fix_sub_problem(t_sub_problem_id, m_last_solutions);

    auto& model = m_formulation.sub_problem(t_sub_problem_id);

    model.optimize();

    const auto status = model.get_status();

    if (status != Optimal && status != Feasible) {
        set_status(status);
        set_reason(NotSpecified);
        terminate();
        return true;
    }

    bool has_changed = m_inner_loop_iterations == 0 || std::abs(m_last_solutions[t_sub_problem_id].objective_value() - model.get_best_obj()) > 1e-4;
    m_last_solutions[t_sub_problem_id] = save_primal(model);

    return has_changed;
}
