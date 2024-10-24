//
// Created by henri on 18.09.24.
//

#include "idol/mixed-integer/optimizers/padm/Optimizers_PADM.h"
#include "idol/general/optimizers/logs.h"

#include <utility>
#include <fstream>

idol::Optimizers::PADM::PADM(const Model& t_model,
                             ADM::Formulation t_formulation,
                             std::vector<idol::ADM::SubProblem>&& t_sub_problem_specs,
                             PenaltyUpdate* t_penalty_update,
                             SolutionStatus t_feasible_solution_status,
                             double t_initial_penalty_parameter,
                             Plots::Manager* t_plot_manager)
                            : Algorithm(t_model),
                              m_formulation(std::move(t_formulation)),
                              m_sub_problem_specs(std::move(t_sub_problem_specs)),
                              m_penalty_update(t_penalty_update),
                              m_feasible_solution_status(t_feasible_solution_status),
                              m_initial_penalty_parameter(t_initial_penalty_parameter),
                              m_history_plotter(t_plot_manager ? new IterationPlot(*t_plot_manager) : nullptr) {

}


double idol::Optimizers::PADM::get_var_result(const idol::Var &t_var,
                                              const std::function<double(const Var &, unsigned int)> &t_function) const {


    const unsigned int sub_problem_id = m_formulation.sub_problem_id(t_var);

    if (sub_problem_id == -1) {
        const unsigned int n_sub_problems = m_formulation.n_sub_problems();
        double max = std::numeric_limits<double>::lowest();
        for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
            max = std::max(max, t_function(t_var, i));
        }
        return max;
    }

    return t_function(t_var, sub_problem_id);
}


double idol::Optimizers::PADM::get_var_primal(const idol::Var &t_var) const {

    return get_var_result(t_var, [this](const Var &t_var, unsigned int t_sub_problem_id) {
        return m_last_solutions[t_sub_problem_id].get(t_var);
    });

}

double idol::Optimizers::PADM::get_var_reduced_cost(const idol::Var &t_var) const {
    return get_var_result(t_var, [this](const Var &t_var, unsigned int t_sub_problem_id) {
        return m_formulation.sub_problem(t_sub_problem_id).get_var_reduced_cost(t_var);
    });
}

double idol::Optimizers::PADM::get_var_ray(const idol::Var &t_var) const {
    return get_var_result(t_var, [this](const Var &t_var, unsigned int t_sub_problem_id) {
        return m_formulation.sub_problem(t_sub_problem_id).get_var_ray(t_var);
    });
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
    m_first_run = true;
    m_n_restart = 0;
    m_last_iteration_with_no_feasibility_change.reset();
    m_last_objective_value_when_rescaled.reset();
    m_current_initial_penalty_parameter = m_initial_penalty_parameter;
    m_last_solutions = std::vector<PrimalPoint>(n_sub_problems);

    for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {

        auto& model = m_formulation.sub_problem(i);
        if (!model.has_optimizer()) {
            model.use(m_sub_problem_specs[i].optimizer_factory());
        }

        m_last_solutions[i] = m_sub_problem_specs[i].initial_point();
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

        bool objective_value_has_changed = false;
        bool feasibility_has_changed = false;

        for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
            const auto [obj, feas] = solve_sub_problem(i);
            objective_value_has_changed |= obj;
            feasibility_has_changed |= feas;
        }

        m_first_run = false;
        ++m_inner_loop_iterations;

        check_time_limit();

        log_inner_loop(inner_loop_iteration);
        make_history();

        if (is_terminated()) {
            break;
        }

        detect_stagnation(feasibility_has_changed);

        if (!objective_value_has_changed) {
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

    bool has_rescaled = m_formulation.update_penalty_parameters(m_last_solutions, *m_penalty_update);

    if (has_rescaled) {
        detect_stagnation_due_to_rescaling();
    }

}

std::pair<bool, bool>
idol::Optimizers::PADM::solve_sub_problem(unsigned int t_sub_problem_id) {

    m_formulation.fix_sub_problem(t_sub_problem_id, m_last_solutions);

    auto& model = m_formulation.sub_problem(t_sub_problem_id);

    model.optimizer().set_param_time_limit(get_remaining_time());
    model.optimize();

    const auto status = model.get_status();

    if (status != Optimal && status != Feasible) {

        const auto& reason = model.get_reason();

        PrimalPoint sub_problem_solution;
        sub_problem_solution.set_status(status);
        sub_problem_solution.set_reason(reason);
        m_last_solutions[t_sub_problem_id] = std::move(sub_problem_solution);

        set_status(status);
        set_reason(reason);

        terminate();

        return { true, false };
    }

    auto current_solution = save_primal(model);
    // bool obj_has_changed = m_inner_loop_iterations == 0 || (m_last_solutions[t_sub_problem_id] + -1. * current_solution).norm(2) > 1e-4;
    const bool obj_has_changed = m_first_run || std::abs(m_last_solutions[t_sub_problem_id].objective_value() - current_solution.objective_value()) > 1e-4;
    const bool feas_has_changed = m_first_run || std::abs(infeasibility_l1(t_sub_problem_id, m_last_solutions[t_sub_problem_id]) - infeasibility_l1(t_sub_problem_id, current_solution)) > 1e-5;
    m_last_solutions[t_sub_problem_id] = std::move(current_solution);

    return { obj_has_changed, feas_has_changed };
}

void idol::Optimizers::PADM::compute_objective_value() {

    const auto& obj = parent().get_obj_expr();

    double result = obj.constant();

    for (const auto& [var, constant] : obj.linear()) {
        result += constant * get_var_primal(var);
    }

    for (const auto& [vars, constant] : obj.quadratic()) {
        result += constant * get_var_primal(vars.first) * get_var_primal(vars.second);
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
        std::cout << std::setw(12) << std::fixed << std::setprecision(8) << infeasibility_l1(i, m_last_solutions[i]) << '\t';
    }

    std::cout << std::endl;
}

double idol::Optimizers::PADM::infeasibility_linf(unsigned int t_sub_problem_id, const PrimalPoint& t_solution) const {

    double result = 0;

    for (const auto& var : m_formulation.l1_vars(t_sub_problem_id)) {
        const double val = t_solution.get(var);
        result = std::max(result, val);
    }

    return result;
}

double idol::Optimizers::PADM::infeasibility_l1(unsigned int t_sub_problem_id, const PrimalPoint& t_solution) const {

    double result = 0;

    for (const auto& var : m_formulation.l1_vars(t_sub_problem_id)) {
        result += t_solution.get(var);
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
            infeasibility_values.emplace_back(infeasibility_l1(i, m_last_solutions[i]));
        }

        if (m_history_plotter) {
            m_history_plotter->update(m_outer_loop_iteration,
                                     m_inner_loop_iterations,
                                     objective_values,
                                     infeasibility_values);
        }

        m_history.emplace_back(m_outer_loop_iteration,
                               m_inner_loop_iterations,
                               std::move(objective_values),
                               std::move(infeasibility_values)
                               );

    }

}

void idol::Optimizers::PADM::detect_stagnation(bool t_feasibility_has_changed) {

    if (t_feasibility_has_changed) {
        m_last_iteration_with_no_feasibility_change.reset();
        return;
    }

    if (!m_last_iteration_with_no_feasibility_change.has_value()) {
        m_last_iteration_with_no_feasibility_change = m_inner_loop_iterations;
        return;
    }

    if (m_inner_loop_iterations - m_last_iteration_with_no_feasibility_change.value() <= m_max_iterations_without_feasibility_change) {
        return;
    }

    restart();

}

void idol::Optimizers::PADM::detect_stagnation_due_to_rescaling() {

    const auto sum_sub_problems = std::accumulate(m_formulation.sub_problems().begin(), m_formulation.sub_problems().end(), 0., [](double t_acc, const Model& t_model) {
        return t_acc + t_model.get_best_obj();
    });

    if (!m_last_objective_value_when_rescaled.has_value()) {
        m_last_objective_value_when_rescaled = sum_sub_problems;
        return;
    }

    if (std::abs(sum_sub_problems - m_last_objective_value_when_rescaled.value()) > 1e-4) {
        m_last_objective_value_when_rescaled = sum_sub_problems;
        return;
    }

    const bool has_diversified = m_penalty_update->diversify();

    if(!has_diversified) {
        restart();
    }

}

void idol::Optimizers::PADM::restart() {

    if (m_n_restart >= 1) {
        set_status(Fail);
        set_reason(Cycling);
        terminate();
        return;
    }

    std::cout << "Restarting with inverse initial penalty parameter..." << std::endl;

    m_last_objective_value_when_rescaled.reset();
    m_last_iteration_with_no_feasibility_change.reset();
    while(m_penalty_update->diversify());
    m_current_initial_penalty_parameter = 1. / m_initial_penalty_parameter;
    m_first_run = true;

    for (unsigned int i = 0 ; i < m_formulation.n_sub_problems() ; ++i) {
        m_last_solutions[i] = m_sub_problem_specs[i].initial_point();
    }

    m_formulation.initialize_penalty_parameters(m_current_initial_penalty_parameter);

    ++m_n_restart;
}

idol::Optimizers::PADM::IterationPlot::IterationPlot(idol::Plots::Manager &t_manager) : m_manager(t_manager) {

}

#ifdef IDOL_USE_ROOT
#include <TGraph.h>
#include <TLine.h>
#include <TAxis.h>
#endif

void idol::Optimizers::PADM::IterationPlot::initialize(unsigned int t_n_sub_problems) {
#ifdef IDOL_USE_ROOT
    // Create the canvas
    m_canvas = m_manager.create<TCanvas>("canvas", "Sub-problems Objective Progress", 800, 600);

    // Create the pads for each sub-problem and TGraphs
    m_pads.resize(t_n_sub_problems);
    m_graphs.resize(t_n_sub_problems);
    m_lines.resize(t_n_sub_problems);

    for (unsigned int i = 0; i < t_n_sub_problems; ++i) {
        // Divide the canvas into vertical pads for each graph
        m_pads[i] = m_manager.create<TPad>(Form("pad%d", i), Form("Sub-problem %d", i), 0.0, 1.0 - (i + 1) * (1.0 / t_n_sub_problems), 1.0, 1.0 - i * (1.0 / t_n_sub_problems));
        m_pads[i]->Draw();
        m_pads[i]->cd();

        // Create a TGraph for each sub-problem
        m_graphs[i] = m_manager.create<TGraph>();
        m_graphs[i]->SetTitle(Form("Sub-problem %d; Inner Loop Iteration; Objective Value", i));

        m_graphs[i]->Draw("APL");

        m_canvas->cd();
    }
#else
    throw Exception("idol was not compiled with ROOT.");
#endif
}

void
idol::Optimizers::PADM::IterationPlot::update(unsigned int t_outer_loop_iteration,
                                              unsigned int t_inner_loop_iteration,
                                              const std::vector<double> &t_objective_values,
                                              const std::vector<double> &t_infeasibilities) {

    if (!m_canvas) {
        initialize(t_objective_values.size());
    }

#ifdef IDOL_USE_ROOT
    const unsigned int n_sub_problems = t_objective_values.size();
    const bool outer_loop_changed = m_last_outer_iteration != t_outer_loop_iteration;

    for (unsigned int i = 0; i < n_sub_problems; ++i) {
        m_graphs[i]->SetPoint(m_graphs[i]->GetN(), t_inner_loop_iteration, t_objective_values[i]);

        m_pads[i]->cd();
        m_graphs[i]->Draw("AL");

        if (outer_loop_changed) {
            auto* vertical_line = m_manager.create<TLine>(t_inner_loop_iteration,
                                                          m_graphs[i]->GetYaxis()->GetXmin(),
                                                          t_inner_loop_iteration,
                                                          m_graphs[i]->GetYaxis()->GetXmax());
            vertical_line->SetLineColor(kRed);
            vertical_line->SetLineStyle(2);  // Dashed line
            m_lines[i].emplace_back(vertical_line);
            m_last_outer_iteration = t_outer_loop_iteration;
        }

        for (auto* line : m_lines[i]) {
            line->SetY1(m_graphs[i]->GetYaxis()->GetXmin());
            line->SetY2(m_graphs[i]->GetYaxis()->GetXmax());
            line->Draw("SAME");
        }

    }

    m_canvas->cd();
    m_canvas->Update();
#endif
}
