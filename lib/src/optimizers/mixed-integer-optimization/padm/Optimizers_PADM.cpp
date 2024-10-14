//
// Created by henri on 18.09.24.
//

#include "idol/optimizers/mixed-integer-optimization/padm/Optimizers_PADM.h"

#include <utility>
#include <fstream>

idol::Optimizers::PADM::PADM(const Model& t_model,
                             ADM::Formulation t_formulation,
                             std::vector<idol::ADM::SubProblem>&& t_sub_problem_specs,
                             PenaltyUpdate* t_penalty_update,
                             SolutionStatus t_feasible_solution_status,
                             double t_initial_penalty_parameter)
                            : Algorithm(t_model),
                              m_formulation(std::move(t_formulation)),
                              m_sub_problem_specs(std::move(t_sub_problem_specs)),
                              m_penalty_update(t_penalty_update),
                              m_feasible_solution_status(t_feasible_solution_status),
                              m_initial_penalty_parameter(t_initial_penalty_parameter) {

}

double idol::Optimizers::PADM::get_var_primal(const idol::Var &t_var) const {
    const unsigned int sub_problem_id = m_formulation.sub_problem_id(t_var);
    return m_last_solutions[sub_problem_id].get(t_var);
}

double idol::Optimizers::PADM::get_var_reduced_cost(const idol::Var &t_var) const {
    const unsigned int sub_problem_id = m_formulation.sub_problem_id(t_var);
    return m_formulation.sub_problem(sub_problem_id).get_var_reduced_cost(t_var);
}

double idol::Optimizers::PADM::get_var_ray(const idol::Var &t_var) const {
    const unsigned int sub_problem_id = m_formulation.sub_problem_id(t_var);
    return m_formulation.sub_problem(sub_problem_id).get_var_ray(t_var);
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

    // if ends with .sol
    if (t_name.size() > 4 && t_name.substr(t_name.size() - 4) == ".sol") {
        write_solution(t_name);
        return;
    }

    // if ends with .iter
    if (t_name.size() > 5 && t_name.substr(t_name.size() - 5) == ".iter") {
        write_iteration_history(t_name);
        return;
    }

    throw Exception("Not implemented");

}

void idol::Optimizers::PADM::write_solution(const std::string &t_name) {

    std::ofstream file(t_name);

    if (!file.is_open()) {
        throw Exception("Could not open file " + t_name);
    }

    file << "# Objective value = " << std::setprecision(12) << get_best_obj() << std::endl;
    for (const auto& var : parent().vars()) {
        file << var.name() << " " << std::setprecision(12) << get_var_primal(var) << std::endl;
    }

    file.close();

}

void idol::Optimizers::PADM::write_iteration_history(const std::string &t_name) {

    std::ofstream file(t_name);

    if (!file.is_open()) {
        throw Exception("Could not open file " + t_name);
    }

    file << "[\n";

    for (auto it = m_history.begin(), end = m_history.end(); it != end; ++it) {
        const auto& log = *it;

        // Writing the "outer", "inner", and "values" fields
        file << "\t{ \"outer\": " << log.outer_iteration
             << ", \"inner\": " << log.inner_iteration
             << ", \"values\": [ ";

        for (unsigned int i = 0, n = log.objective_value.size(); i < n; ++i) {
            file << log.objective_value[i];
            if (i != n - 1) {
                file << ", ";
            }
        }

        // Writing the "infeasibility" field as an array
        file << " ], \"infeasibility\": [ ";

        for (unsigned int i = 0, n = log.infeasibility.size(); i < n; ++i) {
            file << log.infeasibility[i];
            if (i != n - 1) {
                file << ", ";
            }
        }

        file << " ]}";

        // Add a comma except for the last element in the array
        if (std::next(it) != end) {
            file << ",";
        }
        file << "\n";
    }

    file << "]\n";

    file.close();

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

    if (get_param_iteration_limit() == 0) {
        return;
    }

    do {

        log_outer_loop();

        update_penalty_parameters();

        run_inner_loop();

        ++m_outer_loop_iteration;

        if (is_terminated()) {
            break;
        }

        check_outer_iteration_limit();
        check_time_limit();
        check_feasibility();

    } while (!is_terminated());

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

    const unsigned int n_sub_problems = m_formulation.n_sub_problems();

    if (n_sub_problems == 1) {
        solve_sub_problem(0);
        ++m_inner_loop_iterations;
        return;
    }

    for (unsigned int inner_loop_iteration = 0 ; inner_loop_iteration < m_max_inner_loop_iterations ; ++inner_loop_iteration) {

        bool has_changed = false;

        for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
            has_changed |= solve_sub_problem(i);
        }

        ++m_inner_loop_iterations;

        check_time_limit();

        log_inner_loop(inner_loop_iteration);
        make_history();

        if (is_terminated()) {
            break;
        }

        if (!has_changed) {
            break;
        }

    }

}

void idol::Optimizers::PADM::update_penalty_parameters() {

    if (!m_formulation.has_penalized_constraints()) {
        return;
    }

    if (m_inner_loop_iterations == 0) {
        m_formulation.initialize_penalty_parameters(m_initial_penalty_parameter);
        return;
    }

    m_formulation.update_penalty_parameters(m_last_solutions, *m_penalty_update);

}

bool idol::Optimizers::PADM::solve_sub_problem(unsigned int t_sub_problem_id) {

    m_formulation.fix_sub_problem(t_sub_problem_id, m_last_solutions);

    auto& model = m_formulation.sub_problem(t_sub_problem_id);

    model.optimizer().set_param_time_limit(get_remaining_time());
    model.optimize();

    const auto status = model.get_status();

    if (status != Optimal && status != Feasible) {

        const auto& reason = model.get_reason();

        Solution::Primal sub_problem_solution;
        sub_problem_solution.set_status(status);
        sub_problem_solution.set_reason(reason);
        m_last_solutions[t_sub_problem_id] = std::move(sub_problem_solution);

        set_status(status);
        set_reason(reason);

        terminate();

        return true;
    }

    auto current_solution = save_primal(model);
    // bool has_changed = m_inner_loop_iterations == 0 || (m_last_solutions[t_sub_problem_id] + -1. * current_solution).norm(2) > 1e-4;
    const bool has_changed = m_inner_loop_iterations == 0 || std::abs(m_last_solutions[t_sub_problem_id].objective_value() - current_solution.objective_value()) > 1e-4;
    m_last_solutions[t_sub_problem_id] = std::move(current_solution);

    return has_changed;
}

void idol::Optimizers::PADM::compute_objective_value() {

    const auto& obj = parent().get_obj_expr();

    double result = obj.constant().as_numerical();

    for (const auto& [var, constant] : obj.linear()) {
        result += constant.as_numerical() * get_var_primal(var);
    }

    for (const auto& [var1, var2, constant] : obj.quadratic()) {
        result += constant.as_numerical() * get_var_primal(var1) * get_var_primal(var2);
    }

    set_best_obj(result);

}

void idol::Optimizers::PADM::log_inner_loop(unsigned int t_inner_loop_iteration) {

    if (!get_param_logs()) {
        return;
    }

    const unsigned int n_sub_problems = m_formulation.n_sub_problems();

    std::cout << std::setw(10) << std::fixed << std::setprecision(5) << time().count() << '\t'
              << std::setw(5) << m_inner_loop_iterations << '\t'
              << std::setw(5) << m_outer_loop_iteration << '\t'
              << std::setw(5) << t_inner_loop_iteration << '\t';

    for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
        std::cout << std::setw(12) << m_last_solutions[i].status() << '\t';
        std::cout << std::setw(12) << std::fixed << std::setprecision(3) << max_infeasibility(i) << '\t';
    }

    std::cout << std::endl;
}

double idol::Optimizers::PADM::max_infeasibility(unsigned int t_sub_problem_id) const {

    double result = 0;

    for (const auto& var : m_formulation.l1_vars(t_sub_problem_id)) {
        const double val = m_last_solutions[t_sub_problem_id].get(var);
        result = std::max(result, val);
    }

    return result;
}

double idol::Optimizers::PADM::total_infeasibility(unsigned int t_sub_problem_id) const {

    double result = 0;

    for (const auto& var : m_formulation.l1_vars(t_sub_problem_id)) {
        result += m_last_solutions[t_sub_problem_id].get(var);
    }

    return result;
}

void idol::Optimizers::PADM::check_feasibility() {

    if (!is_feasible()) {
        return;
    }

    set_status(m_feasible_solution_status);
    set_reason(Proved);
    compute_objective_value();
    terminate();

    if (m_feasible_solution_status == Optimal) {
        set_best_bound(get_best_obj());
    }

}

void idol::Optimizers::PADM::check_time_limit() {

    if (get_remaining_time() > 0) {
        return;
    }

    set_status(Infeasible);
    set_reason(TimeLimit);
    terminate();

}

void idol::Optimizers::PADM::check_outer_iteration_limit() {

    if (m_outer_loop_iteration < get_param_iteration_limit()) {
        return;
    }

    set_status(Infeasible);
    set_reason(IterLimit);
    terminate();

}

unsigned int idol::Optimizers::PADM::get_outer_loop_iteration_count() const {
    return m_outer_loop_iteration;
}

unsigned int idol::Optimizers::PADM::get_inner_loop_iteration_count() const {
    return m_inner_loop_iterations;
}

void idol::Optimizers::PADM::log_outer_loop() {

    if (!get_param_logs()) {
        return;
    }

    center(std::cout, " Outer loop iteration: " + std::to_string(m_outer_loop_iteration) + ' ', 100, '-');
    std::cout << std::endl;

}

void idol::Optimizers::PADM::make_history() {

    if (!is_terminated()) {

        const unsigned int n_sub_problems = m_formulation.n_sub_problems();

        std::vector<double> objective_values;
        std::vector<double> infeasibility_values;
        for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
            objective_values.emplace_back(m_formulation.sub_problem(i).get_best_obj());
            infeasibility_values.emplace_back(total_infeasibility(i));
        }
        m_history.emplace_back(m_outer_loop_iteration,
                               m_inner_loop_iterations,
                               std::move(objective_values),
                               std::move(infeasibility_values)
                               );

    }

}
