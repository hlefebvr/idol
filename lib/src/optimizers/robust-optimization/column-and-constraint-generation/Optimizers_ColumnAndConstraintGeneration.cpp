//
// Created by henri on 08.02.24.
//

#include <cassert>
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/modeling/objects/Versions.h"
#include "idol/modeling/expressions/operations/operators.h"
#include "idol/modeling/objects/Env.h"

#define N_SECTIONS 3
#define LOG_WIDTH_ITERATION 5
#define LOG_WIDTH_TOTAL_TIME 12
#define LOG_WIDTH_PROBLEM_STATUS 12
#define LOG_WIDTH_ITER_TYPE 7
#define LOG_WIDTH_ITER_STATUS 12
#define LOG_WIDTH_ITER_REASON 12
#define LOG_WIDTH_BEST_BOUND 10
#define LOG_WIDTH_BEST_OBJ 10
#define LOG_WIDTH_REL_GAP 10
#define LOG_WIDTH_ABS_GAP 10
#define LOG_WIDTH_ITER_OBJ 10
#define DOUBLE_PRECISION 5

constexpr auto algorithm_space = (LOG_WIDTH_ITERATION + LOG_WIDTH_TOTAL_TIME);
constexpr auto problem_space = (LOG_WIDTH_PROBLEM_STATUS + LOG_WIDTH_BEST_BOUND + LOG_WIDTH_BEST_OBJ + LOG_WIDTH_REL_GAP + LOG_WIDTH_ABS_GAP);
constexpr auto iteration_space = (LOG_WIDTH_ITER_TYPE + LOG_WIDTH_ITER_STATUS + LOG_WIDTH_ITER_REASON + LOG_WIDTH_ITER_OBJ);
constexpr auto table_space = algorithm_space + problem_space + iteration_space + N_SECTIONS * 3 + 1;

idol::Optimizers::Robust::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(const idol::Model &t_parent,
                                                                               const idol::Model &t_uncertainty_set,
                                                                               const idol::OptimizerFactory &t_master_optimizer,
                                                                               const idol::Robust::ColumnAndConstraintGenerationSeparator &t_separator,
                                                                               const idol::Annotation<idol::Var, unsigned int> &t_lower_level_variables,
                                                                               const idol::Annotation<idol::Ctr, unsigned int> &t_lower_level_constraints,
                                                                               bool t_complete_recourse)
        : Algorithm(t_parent),
          m_uncertainty_set(t_uncertainty_set.copy()),
          m_lower_level_variables(t_lower_level_variables),
          m_lower_level_constraints(t_lower_level_constraints),
          m_master_problem(t_parent.env()),
          m_separator(t_separator.clone()),
          m_complete_recourse(t_complete_recourse) {

    build_master_problem();
    build_coupling_constraints_list();
    build_upper_and_lower_level_variables_list();
    build_upper_and_lower_level_constraints_list();

    m_master_problem.use(t_master_optimizer);

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::build_master_problem() {

    build_master_problem_variables();
    build_master_problem_constraints();
    build_master_problem_objective_and_epigraph();

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::build_master_problem_variables() {

    const auto& parent = this->parent();

    for (const auto& var : parent.vars()) {

        if (var.get(m_lower_level_variables) != MasterId) {
            continue;
        }

        const double lb = parent.get_var_lb(var);
        const double ub = parent.get_var_ub(var);
        const auto type = parent.get_var_type(var);

        m_master_problem.add(var, TempVar(lb, ub, type, Column()));

    }

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::build_master_problem_constraints() {

    const auto& parent = this->parent();

    for (const auto& ctr : parent.ctrs()) {

        if (ctr.get(m_lower_level_constraints) != MasterId) {
            continue;
        }

        const auto& row = parent.get_ctr_row(ctr);

        if (contains_lower_level_variable(row.linear()) || contains_lower_level_variable(row.quadratic())) {
            continue;
        }

        const auto type = parent.get_ctr_type(ctr);

        m_master_problem.add(ctr, TempCtr(Row(row), type));

    }

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::build_master_problem_objective_and_epigraph() {

    const auto& parent = this->parent();
    auto& env = m_master_problem.env();

    const auto& objective = parent.get_obj_expr();

    Expr upper_level_variables_part = objective.constant();
    Expr lower_level_variables_part;
    for (const auto& [var, constant] : objective.linear()) {

        if (var.get(m_lower_level_variables) == MasterId) {
            upper_level_variables_part += constant * var;
        } else {
            lower_level_variables_part += constant * var;
        }

    }

    for (const auto& [var1, var2, constant] : objective.quadratic()) {

        const auto id1 = var1.get(m_lower_level_variables);
        const auto id2 = var2.get(m_lower_level_variables);

        if (id1 != MasterId || id2 != MasterId) {
            lower_level_variables_part += constant * var1 * var2;
        } else {
            upper_level_variables_part += constant * var1 * var2;
        }

    }

    m_master_problem.set_obj_expr(upper_level_variables_part);

    if (lower_level_variables_part.is_zero()) {
        return;
    }

    auto epigraph_variable = Var(env, -Inf, Inf, Continuous, "__epigraph_var");
    auto epigraph_constraint = Ctr(env, epigraph_variable >= lower_level_variables_part, "__epigraph_ctr");
    m_epigraph = std::make_optional<std::pair<Var, Ctr>>(epigraph_variable, epigraph_constraint);

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::build_coupling_constraints_list() {

    const auto& parent = this->parent();

    m_coupling_constraints.clear();

    for (const auto& ctr : parent.ctrs()) {

        if (ctr.get(m_lower_level_constraints) != MasterId) {
            continue;
        }

        const auto& row = parent.get_ctr_row(ctr);

        if (!contains_lower_level_variable(row.linear()) && !contains_lower_level_variable(row.quadratic())) {
            continue;
        }

        m_coupling_constraints.emplace_back(ctr);

    }

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::build_upper_and_lower_level_variables_list() {

    const auto& parent = this->parent();

    m_upper_level_variables_list.clear();
    m_lower_level_variables_list.clear();

    for (const auto& var : parent.vars()) {

        if (var.get(m_lower_level_variables) != MasterId) {
            m_lower_level_variables_list.emplace_back(var);
        } else {
            m_upper_level_variables_list.emplace_back(var);
        }

    }

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::build_upper_and_lower_level_constraints_list() {

    const auto& parent = this->parent();

    m_upper_level_constraints_list.clear();
    m_lower_level_constraints_list.clear();

    for (const auto& ctr : parent.ctrs()) {

        if (ctr.get(m_lower_level_constraints) != MasterId) {
            m_lower_level_constraints_list.emplace_back(ctr);
        } else {
            m_upper_level_constraints_list.emplace_back(ctr);
        }

    }

}

std::string idol::Optimizers::Robust::ColumnAndConstraintGeneration::name() const {
    return "column-and-constraint generation";
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_var_primal(const Var &t_var) const {

    if (t_var.get(m_lower_level_variables) != MasterId) {
        return 0.;
    }

    return m_master_problem.get_var_primal(t_var);
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_var_ray(const Var &t_var) const {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_ctr_dual(const Ctr &t_ctr) const {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_ctr_farkas(const Ctr &t_ctr) const {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

unsigned int idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_n_solutions() const {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

unsigned int idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_solution_index() const {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::add(const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::add(const Ctr &t_ctr) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::remove(const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::remove(const Ctr &t_ctr) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update() {

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::write(const std::string &t_name) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::hook_before_optimize() {
    Optimizer::hook_before_optimize();

    if (get_param_logs()) {

        const auto& parent = this->parent();

        const unsigned int n_master_constraints = m_master_problem.ctrs().size();
        const unsigned int n_coupling_constraints = m_coupling_constraints.size();
        const unsigned int n_lower_level_constraints = parent.ctrs().size() - n_master_constraints - n_coupling_constraints;
        const unsigned int n_upper_level_variables = m_master_problem.vars().size();
        const unsigned int n_lower_level_variables = parent.vars().size() - n_upper_level_variables;

        std::cout << "------------------------------------\n";
        std::cout << "* Solving Two-Stage Robust Problem *\n";
        std::cout << "------------------------------------\n\n";

        std::cout << "Variables: \n";
        std::cout << "\tN. upper level: " << n_upper_level_variables << '\n';
        std::cout << "\tN. lower level: " << n_lower_level_variables << '\n';

        std::cout << "Constraints: \n";
        std::cout << "\tN. pure upper level: " << n_master_constraints << '\n';
        std::cout << "\tN. coupling: " << (n_coupling_constraints + m_epigraph.has_value()) << '\n';
        std::cout << "\tN. lower level: " << n_lower_level_constraints << '\n';

        std::cout << std::endl;
    }

    set_best_bound(-Inf);
    set_best_obj(+Inf);
    set_status(Loaded);
    set_reason(NotSpecified);

    m_iteration_count = 0;
    m_last_scenario = Solution::Primal();

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::hook_optimize() {

    log_init();

    while (!is_terminated()) {

        solve_master_problem();

        analyze_master_problem_solution();

        log(true);

        if (is_terminated()) { break; }

        m_separation_timer.start();
        m_last_scenario = solve_separation_problems();
        m_separation_timer.stop();

        if (is_terminated()) { break; }

        analyze_last_separation();

        log(false);

        if (is_terminated()) { break; }

        add_scenario(m_last_scenario);

        check_stopping_condition();

        ++m_iteration_count;

    }

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::set_solution_index(unsigned int t_index) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_obj_sense() {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_obj() {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_rhs() {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_obj_constant() {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_mat_coeff(const Ctr &t_ctr, const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_ctr_type(const Ctr &t_ctr) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_ctr_rhs(const Ctr &t_ctr) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_var_type(const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_var_lb(const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_var_ub(const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::update_var_obj(const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

bool
idol::Optimizers::Robust::ColumnAndConstraintGeneration::contains_lower_level_variable(const idol::LinExpr<idol::Var> &t_expr) {

    for (const auto& [var, constant] : t_expr) {

        if (var.get(m_lower_level_variables) != MasterId) {
            return true;
        }

    }

    return false;
}

bool idol::Optimizers::Robust::ColumnAndConstraintGeneration::contains_lower_level_variable(
        const idol::QuadExpr<idol::Var, idol::Var> &t_expr) {


    for (const auto& [var1, var2, constant] : t_expr) {

        if (var1.get(m_lower_level_variables) != MasterId || var2.get(m_lower_level_variables) != MasterId) {
            return true;
        }

    }

    return false;

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_master_problem() {
    m_master_problem.optimize();
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::analyze_master_problem_solution() {

    const auto status = m_master_problem.get_status();

    if (status == Unbounded) {
        throw Exception("Not implemented: cannot handle unbounded master problem, yet.");
    }

    if (status == Infeasible) {
        set_status(Infeasible);
        set_reason(Proved);
        terminate();
        return;
    }

    if (status != Optimal) {
        const auto reason = m_master_problem.get_reason();
        set_status(status);
        set_reason(reason);
        terminate();
        return;
    }

    const double objective = m_master_problem.get_best_obj();

    set_best_bound(objective);
    set_status(SubOptimal);

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::check_stopping_condition() {

    if (get_remaining_time() < 0) {
        set_reason(TimeLimit);
        terminate();
        return;
    }

    if (m_iteration_count >= get_param_iteration_limit()) {
        set_reason(IterLimit);
        terminate();
        return;
    }

    if (get_relative_gap() < get_tol_mip_relative_gap()
        || get_absolute_gap() < get_tol_mip_absolute_gap()) {
        set_status(Optimal);
        set_reason(Proved);
        terminate();
        return;
    }

}

idol::Solution::Primal
idol::Optimizers::Robust::ColumnAndConstraintGeneration::solve_separation_problems() {

    const auto& parent = this->parent();
    Solution::Primal upper_level_solution = save_upper_level_primal();

    double max = -Inf;
    Solution::Primal argmax;
    argmax.set_objective_value(-Inf);

    const auto evaluate = [&](const Row& t_row, CtrType t_type) {

        auto solution = m_separator->operator()(*this, upper_level_solution, t_row, t_type);

        /*
        std::cout << "SEPARATION SOLUTION "
                  << "\n************************\n"
                  << solution
                  << "\n************************\n"
                  << std::endl;
        */

        const auto status = solution.status();

        if (status != Optimal) {
            set_status(status);
            set_reason(solution.reason());
            terminate();
            return;
        }

        const double objective_value = solution.objective_value();

        if (max < objective_value) {
            max = objective_value;
            argmax = solution;
        }

    };

    if (m_epigraph.has_value()) {

        const auto& env = m_master_problem.env();
        const auto& default_version = env.operator[](m_epigraph->second);
        const auto& row = default_version.row();
        const auto type = default_version.type();

        evaluate(row, type);

    }

    if (is_terminated()) {
        return argmax;
    }

    for (const auto& ctr : m_coupling_constraints) {

        const auto& row = parent.get_ctr_row(ctr);
        const auto type = parent.get_ctr_type(ctr);

        evaluate(row, type);

        if (is_terminated()) {
            break;
        }

    }

    // If there are no coupling constraint, this is a feasibility problem, i.e., we optimize 0.
    if (m_coupling_constraints.empty() && !m_epigraph.has_value()) {
        evaluate(Row(), LessOrEqual);
    }

    return argmax;
}

idol::Solution::Primal idol::Optimizers::Robust::ColumnAndConstraintGeneration::save_upper_level_primal() const {

    Solution::Primal result;

    const auto status = m_master_problem.get_status();
    const auto reason = m_master_problem.get_reason();
    const auto objective_value = m_master_problem.get_best_obj();

    result.set_status(status);
    result.set_reason(reason);
    result.set_objective_value(objective_value);

    if (m_epigraph.has_value() && m_master_problem.has(m_epigraph->first)) {
        const auto& epigraph_var = m_epigraph->first;
        const double value = m_master_problem.get_var_primal(epigraph_var);
        result.set(epigraph_var, value);
    }

    for (const auto& var : m_upper_level_variables_list) {
        const double value = m_master_problem.get_var_primal(var);
        result.set(var, value);
    }

    return result;
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::analyze_last_separation() {

    const auto status = m_last_scenario.status();

    if (status != Optimal) {
        set_status(status);
        set_reason(m_last_scenario.reason());
        terminate();
        return;
    }

    // TODO for <= only

    if (m_last_scenario.objective_value() <= Tolerance::Feasibility) {
        set_status(Optimal);
        set_reason(Proved);
        set_best_obj(get_best_bound());
        terminate();
        return;
    }

}

void
idol::Optimizers::Robust::ColumnAndConstraintGeneration::add_scenario(const idol::Solution::Primal &t_most_violated_scenario) {

    const auto& parent = this->parent();

    const unsigned int n_variables = parent.vars().size();

    std::vector<std::unique_ptr<Var>> variables(n_variables);

    // Create new lower level variables
    for (const auto& var : m_lower_level_variables_list) {

        const unsigned int index = parent.get_var_index(var);
        const double lb = parent.get_var_lb(var);
        const double ub = parent.get_var_ub(var);
        const auto type = parent.get_var_type(var);

        auto new_var = m_master_problem.add_var(lb, ub, type, var.name() + "__" + std::to_string(m_iteration_count));

        variables[index] = std::make_unique<Var>(new_var);

    }

    const auto add_lower_level_constraint = [&](const Ctr& t_ctr, const Row& t_row, CtrType t_type) {

        Expr<Var, Var> lhs;
        Constant rhs = t_row.rhs().fix(t_most_violated_scenario);

        for (const auto& [var, constant] : t_row.linear()) {

            if (var.get(m_lower_level_variables) == MasterId) {
                lhs += constant.fix(t_most_violated_scenario) * var;
                continue;
            }

            const unsigned int index = parent.get_var_index(var);

            lhs += constant.fix(t_most_violated_scenario) * *variables[index];

        }

        m_master_problem.add_ctr(TempCtr(Row(lhs, rhs), t_type), t_ctr.name() + "__" + std::to_string(m_iteration_count));


    };

    // Create new lower level constraints
    for (const auto& ctr : m_lower_level_constraints_list) {

        const auto& row = parent.get_ctr_row(ctr);
        const auto type = parent.get_ctr_type(ctr);

        add_lower_level_constraint(ctr, row, type);

    }

    // Create new coupling constraints
    for (const auto& ctr : m_coupling_constraints) {

        const auto& row = parent.get_ctr_row(ctr);
        const auto type = parent.get_ctr_type(ctr);

        add_lower_level_constraint(ctr, row, type);

    }

    // Create new epigraph constraint (if needed)
    if (m_epigraph) {

        if (!m_master_problem.has(m_epigraph->first)) {
            m_master_problem.add(m_epigraph->first, TempVar(-Inf, Inf, Continuous, Column(1)));
        }

        const auto& env = m_master_problem.env();
        const auto& default_version = env[m_epigraph->second];
        const auto& row = default_version.row();
        const auto type = default_version.type();

        add_lower_level_constraint(m_epigraph->second, row, type);

    }

}

double idol::Optimizers::Robust::ColumnAndConstraintGeneration::get_var_reduced_cost(const idol::Var &t_var) const {
    throw Exception("Not implemented ColumnAndConstraintGeneration::get_var_reduced_cost.");
}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::log(bool t_is_rmp) const {

    if (!get_param_logs()) {
        return;
    }

    const std::string iter_type = t_is_rmp ? "RMP" : "SP";

    const SolutionStatus iter_status = t_is_rmp ? m_master_problem.get_status() : m_last_scenario.status();
    const SolutionReason iter_reason = t_is_rmp ? m_master_problem.get_reason() : m_last_scenario.reason();
    const double iter_obj = t_is_rmp ? m_master_problem.get_best_obj() : m_last_scenario.objective_value();

    std::cout
            << " | "
            << std::setw(LOG_WIDTH_ITERATION) << m_iteration_count
            << std::setw(LOG_WIDTH_TOTAL_TIME) << parent().optimizer().time().count()  << " | "
            << std::setw(LOG_WIDTH_PROBLEM_STATUS) << get_status()
            << std::setw(LOG_WIDTH_BEST_BOUND) << pretty_double(get_best_bound(), DOUBLE_PRECISION)
            << std::setw(LOG_WIDTH_BEST_OBJ) << pretty_double(get_best_obj(), DOUBLE_PRECISION)
            << std::setw(LOG_WIDTH_REL_GAP) << pretty_double(get_relative_gap() * 100., DOUBLE_PRECISION)
            << std::setw(LOG_WIDTH_ABS_GAP) << pretty_double(get_absolute_gap(), DOUBLE_PRECISION)  << " | "
            << std::setw(LOG_WIDTH_ITER_TYPE) << iter_type
            << std::setw(LOG_WIDTH_ITER_STATUS) << iter_status
            << std::setw(LOG_WIDTH_ITER_REASON) << iter_reason
            << std::setw(LOG_WIDTH_ITER_OBJ) << iter_obj << " | "
            << std::endl;

}

void idol::Optimizers::Robust::ColumnAndConstraintGeneration::log_init() const {

    if (!get_param_logs()) {
        return;
    }

    std::cout << ' ';center(std::cout, "*", table_space, '*') << '\n';
    std::cout << ' ';center(std::cout, " Column-and-Constraint Generation ", table_space) << '\n';
    std::cout << ' ';center(std::cout, "*", table_space, '*') << '\n';

    std::cout << " | ";
    center(std::cout, "Algorithm", algorithm_space);std::cout << " | ";
    center(std::cout, "Problem", problem_space);std::cout << " | ";
    center(std::cout, "Iteration", iteration_space);std::cout << " | ";
    std::cout << '\n';

    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_ITERATION) << "Iter.";
    std::cout << std::setw(LOG_WIDTH_TOTAL_TIME) << "Time";

    // Problem
    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_PROBLEM_STATUS) << "Status";
    std::cout << std::setw(LOG_WIDTH_BEST_BOUND) << "Bound";
    std::cout << std::setw(LOG_WIDTH_BEST_OBJ) << "Obj";
    std::cout << std::setw(LOG_WIDTH_REL_GAP) << "Gap (%)";
    std::cout << std::setw(LOG_WIDTH_ABS_GAP) << "Abs.";

    // Iteration
    std::cout << " | ";
    std::cout << std::setw(LOG_WIDTH_ITER_TYPE) << "Type";
    std::cout << std::setw(LOG_WIDTH_ITER_STATUS) << "Status";
    std::cout << std::setw(LOG_WIDTH_ITER_REASON) << "Reason";
    std::cout << std::setw(LOG_WIDTH_ITER_OBJ) << "Value";
    std::cout << " | ";

    std::cout << std::endl;

}
