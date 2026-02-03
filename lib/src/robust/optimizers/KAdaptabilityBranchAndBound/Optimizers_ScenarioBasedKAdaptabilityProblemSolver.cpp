//
// Created by henri on 12.04.25.
//
#include "idol/robust/optimizers/KAdaptabilityBranchAndBound/Optimizers_ScenarioBasedKAdaptabilityProblemSolver.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"
#include <cassert>

idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::ScenarioBasedKAdaptabilityProblemSolver(
        const idol::Model &t_parent,
        const idol::Bilevel::Description& t_bilevel_description,
        const idol::Robust::Description& t_robust_description,
        unsigned int t_K,
        const idol::OptimizerFactory &t_main_optimizer,
        const OptimizerFactory& t_separation_optimizer)
        : Algorithm(t_parent),
          m_bilevel_description(t_bilevel_description),
          m_robust_description(t_robust_description),
          m_K(t_K),
          m_main_optimizer(t_main_optimizer.clone()),
          m_separation_optimizer(t_separation_optimizer.clone()) {

    build_master_if_not_exists();

}

std::string idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::name() const {
    return "scenario-based-k-solver";
}

double idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::get_var_primal(const idol::Var &t_var) const {

    if (!m_has_first_stage_decisions) {
        throw Exception("Not implemented");
    }

    return m_master_problem.front()->get_var_primal(get_kadapt_var(t_var, m_solution_index));
}

double
idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::get_var_reduced_cost(const idol::Var &t_var) const {
    throw Exception("Not implemented.");
}

double idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::get_var_ray(const idol::Var &t_var) const {
    throw Exception("Not implemented.");
}

double idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented.");
}

double idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw Exception("Not implemented.");
}

unsigned int idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::get_n_solutions() const {
    return m_K;
}

unsigned int idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::get_solution_index() const {
    return m_solution_index;
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::add(const idol::Var &t_var) {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::add(const idol::Ctr &t_ctr) {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::add(const idol::QCtr &t_ctr) {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::remove(const idol::Var &t_var) {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::remove(const idol::Ctr &t_ctr) {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::remove(const idol::QCtr &t_ctr) {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::update() {
    build_master_if_not_exists();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::write(const std::string &t_name) {
    for (unsigned int i = 0 ; i < m_master_problem.size() ; ++i) {
        m_master_problem[i]->write(t_name + "_" + std::to_string(i));
    }
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::set_solution_index(unsigned int t_index) {
    m_solution_index = t_index;
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::update_obj_sense() {
    for (auto& master : m_master_problem) {
        master->set_obj_sense(parent().get_obj_sense());
    }
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::update_obj() {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::update_rhs() {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::update_obj_constant() {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::update_mat_coeff(const idol::Ctr &t_ctr,
                                                                                         const idol::Var &t_var) {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::update_ctr_type(const idol::Ctr &t_ctr) {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::update_ctr_rhs(const idol::Ctr &t_ctr) {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::update_var_type(const idol::Var &t_var) {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::update_var_lb(const idol::Var &t_var) {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::update_var_ub(const idol::Var &t_var) {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::update_var_obj(const idol::Var &t_var) {
    m_master_problem.clear();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::hook_before_optimize() {

    Optimizer::hook_before_optimize();
    build_master_if_not_exists();

    set_solution_index(0);
    set_status(Loaded);
    set_reason(NotSpecified);
    set_best_bound(-Inf);
    set_best_obj(Inf);
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::hook_optimize() {

    if (!m_has_first_stage_decisions) {
        throw Exception("Not implemented");
    }

    auto& master = *m_master_problem.front();
    master.optimizer().set_param_time_limit(get_param_time_limit());
    master.optimize();

    set_status(master.get_status());
    set_reason(master.get_reason());
    set_best_obj(master.get_best_obj());
    set_best_bound(master.get_best_bound());

}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::make_second_stage_variables_duplicates() {

    const auto& parent = this->parent();
    auto& env = parent.env();

    const unsigned int n_vars = parent.vars().size();
    m_y_k = std::vector<std::vector<std::optional<Var>>>(m_K, std::vector<std::optional<Var>>(n_vars));

    for (const auto& var : parent.vars()) {

        if (m_bilevel_description.is_upper(var)) {
            continue;
        }

        const double lb = parent.get_var_lb(var);
        const double ub = parent.get_var_ub(var);
        const auto type = parent.get_var_type(var);
        const auto index = parent.get_var_index(var);

        for (unsigned int k = 0 ; k < m_K ; ++k) {
            m_y_k[k][index] = Var(env, lb, ub, type, 0, var.name() + "_" + std::to_string(k));
        }

    }

}

const idol::Var &idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::get_kadapt_var(const idol::Var &t_var, unsigned int t_k) const {

    if (m_bilevel_description.is_upper(t_var)) {
        return t_var;
    }

    return m_y_k[t_k][parent().get_var_index(t_var)].value();
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::build_master_if_not_exists() {

    if (!m_master_problem.empty() && m_master_problem.front()) {
        return;
    }

    make_second_stage_variables_duplicates();
    make_epigraph_variable_if_not_exists();

    const auto& parent = this->parent();
    auto& env = parent.env();

    if (!m_has_first_stage_decisions) {
        throw Exception("Not implemented");
    }

    m_master_problem = std::vector<std::unique_ptr<Model>>(1);
    m_master_problem.front() = std::make_unique<Model>(env);

    add_variables_to_master();
    add_deterministic_constraints_to_master();
    add_first_stage_objective();

    m_master_problem.front()->use(*m_main_optimizer);

    m_scenario_based_constraints = std::vector<std::list<Ctr>>(m_K);
}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::add_variables_to_master() {

    const auto& parent = this->parent();
    auto& master = *m_master_problem.front();

    // Add variables and their duplicates
    for (const auto& var : parent.vars()) {

        if (m_bilevel_description.is_upper(var)) {
            const double lb = parent.get_var_lb(var);
            const double ub = parent.get_var_ub(var);
            const auto type = parent.get_var_type(var);
            master.add(var, TempVar(lb, ub, type, 0, LinExpr<Ctr>()));
            continue;
        }

        for (unsigned int k = 0 ; k < m_K ; ++k) {
            const auto index = parent.get_var_index(var);
            master.add(*m_y_k[k][index]);
        }

    }

}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::add_deterministic_constraints_to_master() {

    const auto& parent = this->parent();
    auto& master = *m_master_problem.front();

    m_uncertain_constraints.clear();

    // Add constraints
    for (const auto& ctr : parent.ctrs()) {

        const auto type = parent.get_ctr_type(ctr);
        const double rhs = parent.get_ctr_rhs(ctr);
        auto row = parent.get_ctr_row(ctr);

        if (m_bilevel_description.is_upper(ctr)) {
            master.add(ctr, TempCtr(std::move(row), type, rhs));
            continue;
        }

        if (!m_robust_description.uncertain_mat_coeffs(ctr).empty() || !m_robust_description.uncertain_rhs(ctr).empty()) {
            m_uncertain_constraints.emplace_back(ctr);
            continue;
        }

        for (unsigned int k = 0 ; k < m_K ; ++k) {

            LinExpr<Var> lhs;
            for (const auto& [var, coeff] : row) {
                lhs += coeff * get_kadapt_var(var, k);
            }
            master.add_ctr(TempCtr(std::move(lhs), type, rhs), ctr.name() + "_" + std::to_string(k));

        }

    }

}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::add_first_stage_objective() {

    if (!m_has_first_stage_decisions) {
        return;
    }

    const auto& parent = this->parent();
    const auto& objective = parent.get_obj_expr();

    QuadExpr<Var> first_stage_objective = objective.affine().constant();
    for (const auto& [var, coeff] : objective.affine().linear()) {
        if (m_bilevel_description.is_upper(var)) {
            first_stage_objective += var * coeff;
        } else {
            m_second_stage_objective += var * coeff;
        }
    }

    m_master_problem.front()->set_obj_expr(std::move(first_stage_objective));

}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::set_uncertainty_disjunction(const std::vector<std::list<PrimalPoint>> &t_disjunctions) {

    assert(t_disjunctions.size() == m_K);

    for (unsigned int k = 0 ; k < m_K ; ++k) {
        set_uncertainty_disjunction(k, t_disjunctions[k]);
    }

}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::set_uncertainty_disjunction(unsigned int t_k, const std::list<PrimalPoint> &t_disjunction) {

    const auto& parent = this->parent();

    Model* master;
    if (!m_has_first_stage_decisions) {
        master = m_master_problem[t_k].get();
    } else {
        master = m_master_problem.front().get();
    }

    // Remove currently present constraints
    for (const auto& ctr : m_scenario_based_constraints[t_k]) {
        master->remove(ctr);
    }
    m_scenario_based_constraints[t_k].clear();

    // If the disjunction is empty, remove the epigraph variable and return
    if (t_disjunction.empty()) {
        return;
    }

    // Add epigraph constraint
    if (!m_second_stage_objective.empty_all()) {

        std::cout << "ADD Epigraph constraint" << std::endl;

        if (!master->has(*m_epigraph_variable)) {
            std::cout << "Add epigraph variable" << std::endl;
            master->add(*m_epigraph_variable);
            master->set_var_obj(*m_epigraph_variable, 1);
        }

        for (const auto& scenario : t_disjunction) {

            std::cout << "Add scenario" << std::endl;

            LinExpr<Var> lhs;
            for (const auto& [var, coeff] : m_second_stage_objective.affine().linear()) {
                const auto unc_coeff = evaluate(m_robust_description.uncertain_obj(var), scenario);
                lhs += (coeff + unc_coeff) * get_kadapt_var(var, t_k);
            }
            lhs -= *m_epigraph_variable;

            const auto c = master->add_ctr(TempCtr(std::move(lhs), LessOrEqual, 0));
            m_scenario_based_constraints[t_k].emplace_back(c);

        }

    }

    // Add uncertain constraints
    for (const auto& ctr : m_uncertain_constraints) {

        const auto type = parent.get_ctr_type(ctr);
        const auto& row = parent.get_ctr_row(ctr);

        for (const auto &scenario: t_disjunction) {

            LinExpr<Var> lhs;
            for (const auto& [var, coeff] : row) {
                const auto unc_coeff = evaluate(m_robust_description.uncertain_mat_coeff(ctr, var), scenario);
                lhs += (coeff + unc_coeff) * get_kadapt_var(var, t_k);
            }

            const double rhs = parent.get_ctr_rhs(ctr) + evaluate(m_robust_description.uncertain_rhs(ctr), scenario);

            const auto c = master->add_ctr(TempCtr(std::move(lhs), type, rhs));
            m_scenario_based_constraints[t_k].emplace_back(c);

        }

    }

}

void idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver::make_epigraph_variable_if_not_exists() {

    if (m_epigraph_variable) {
        return;
    }

    auto& env = parent().env();
    m_epigraph_variable = Var(env, -Inf, Inf, Continuous, 0, "epigraph");

}
