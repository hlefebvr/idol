//
// Created by Henri on 22/04/2026.
//
#include "idol/bilevel/optimizers/PADM/Optimizers_PADM.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/bilevel/optimizers/StrongDuality/StrongDuality.h"
#include "idol/mixed-integer/optimizers/padm/PADM.h"

idol::Optimizers::Bilevel::PADM::PADM(const Model& t_parent,
                                      const idol::Bilevel::Description &t_description,
                                      const OptimizerFactory &t_deterministic_optimizer)
        : Algorithm(t_parent),
          m_description(t_description),
          m_deterministic_optimizer(t_deterministic_optimizer.clone()),
          m_decomposition(t_parent.env(), "decomposition"),
          m_initial_penalty_parameters(t_parent.env(), "initial_penalty_parameters") {

}

std::string idol::Optimizers::Bilevel::PADM::name() const {
    return "PADM";
}

double idol::Optimizers::Bilevel::PADM::get_var_primal(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    return m_single_level_reformulation->get_var_primal(t_var);
}

double idol::Optimizers::Bilevel::PADM::get_var_reduced_cost(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    return m_single_level_reformulation->get_var_reduced_cost(t_var);
}

double idol::Optimizers::Bilevel::PADM::get_var_ray(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    return m_single_level_reformulation->get_var_ray(t_var);
}

double idol::Optimizers::Bilevel::PADM::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw_if_no_deterministic_model();
    return m_single_level_reformulation->get_ctr_dual(t_ctr);
}

double idol::Optimizers::Bilevel::PADM::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw_if_no_deterministic_model();
    return m_single_level_reformulation->get_ctr_farkas(t_ctr);
}

unsigned int idol::Optimizers::Bilevel::PADM::get_n_solutions() const {
    throw_if_no_deterministic_model();
    return m_single_level_reformulation->get_n_solutions();
}

unsigned int idol::Optimizers::Bilevel::PADM::get_solution_index() const {
    throw_if_no_deterministic_model();
    return m_single_level_reformulation->get_solution_index();
}

void idol::Optimizers::Bilevel::PADM::add(const idol::Var &t_var) {
    m_single_level_reformulation.reset();
}

void idol::Optimizers::Bilevel::PADM::add(const idol::Ctr &t_ctr) {
    m_single_level_reformulation.reset();
}

void idol::Optimizers::Bilevel::PADM::add(const idol::QCtr &t_ctr) {
    m_single_level_reformulation.reset();
}

void idol::Optimizers::Bilevel::PADM::remove(const idol::Var &t_var) {
    m_single_level_reformulation.reset();
}

void idol::Optimizers::Bilevel::PADM::remove(const idol::Ctr &t_ctr) {
    m_single_level_reformulation.reset();
}

void idol::Optimizers::Bilevel::PADM::remove(const idol::QCtr &t_ctr) {
    m_single_level_reformulation.reset();
}

void idol::Optimizers::Bilevel::PADM::update() {
    m_single_level_reformulation.reset();
}

void idol::Optimizers::Bilevel::PADM::write(const std::string &t_name) {
    throw_if_no_deterministic_model();
    m_single_level_reformulation->write(t_name);
}


void idol::Optimizers::Bilevel::PADM::build_decomposition() {

    if (true) {
        const auto& model = parent();
        std::cout << "À la Lefebvre and Schmidt." << std::endl;
        for (const auto& var : m_single_level_reformulation->vars()) {
            if (model.has(var) && m_description.is_upper(var)) {
                var.set(m_decomposition, 1);
            } else {
                var.set(m_decomposition, 0);
            }
        }
    }

    // Set initial penalty parameters for all constraints
    std::cout << "To do; when switching to l1 norm, make sure that abs__ are not penalized..." << std::endl;
    for (const auto& ctr : m_single_level_reformulation->ctrs()) {
        ctr.set(m_initial_penalty_parameters, 1e3);
    }
    for (const auto& qctr : m_single_level_reformulation->qctrs()) {
        qctr.set(m_initial_penalty_parameters, 1e3);
    }

}

void idol::Optimizers::Bilevel::PADM::hook_optimize() {

    if (!m_single_level_reformulation) {

        m_single_level_reformulation = std::make_unique<Model>(idol::Bilevel::StrongDuality::make_model(parent(), m_description));

        build_decomposition();

        auto padm = idol::PADM(m_decomposition, m_initial_penalty_parameters);
        padm.with_rescaling_threshold(1e8);
        const std::vector<PenaltyUpdate*> rules = {
            new PenaltyUpdates::Multiplicative(2),
            new PenaltyUpdates::Multiplicative(1.5),
            new PenaltyUpdates::Multiplicative(2, true),
            new PenaltyUpdates::Additive(500),
        };
        padm.with_penalty_update(PenaltyUpdates::Adaptive(rules));
        padm.with_default_sub_problem_spec(ADM::SubProblem().with_optimizer(*m_deterministic_optimizer));

        m_single_level_reformulation->use(padm);
    }

    // Parameters
    m_single_level_reformulation->optimizer().set_param_logs(get_param_logs());
    m_single_level_reformulation->optimizer().set_param_time_limit(get_param_time_limit());
    m_single_level_reformulation->optimizer().set_param_threads(get_param_thread_limit());
    m_single_level_reformulation->optimizer().set_param_best_bound_stop(get_param_best_bound_stop());
    m_single_level_reformulation->optimizer().set_param_best_obj_stop(get_param_best_obj_stop());
    m_single_level_reformulation->optimizer().set_param_presolve(get_param_presolve());
    m_single_level_reformulation->optimizer().set_param_infeasible_or_unbounded_info(get_param_infeasible_or_unbounded_info());

    // Tolerances
    m_single_level_reformulation->optimizer().set_tol_feasibility(get_tol_feasibility());
    m_single_level_reformulation->optimizer().set_tol_integer(get_tol_integer());
    m_single_level_reformulation->optimizer().set_tol_mip_absolute_gap(get_tol_mip_absolute_gap());
    m_single_level_reformulation->optimizer().set_tol_mip_relative_gap(get_tol_mip_relative_gap());
    m_single_level_reformulation->optimizer().set_tol_optimality(get_tol_optimality());

    m_single_level_reformulation->optimize();

}

void idol::Optimizers::Bilevel::PADM::set_solution_index(unsigned int t_index) {
    throw_if_no_deterministic_model();
    m_single_level_reformulation->set_solution_index(t_index);
}

void idol::Optimizers::Bilevel::PADM::update_obj_sense() {
    m_single_level_reformulation.reset();
}

void idol::Optimizers::Bilevel::PADM::update_obj() {
    m_single_level_reformulation.reset();
}

void idol::Optimizers::Bilevel::PADM::update_rhs() {
    m_single_level_reformulation.reset();
}

void idol::Optimizers::Bilevel::PADM::update_obj_constant() {
    throw_if_no_deterministic_model();
    m_single_level_reformulation->set_obj_const(parent().get_obj_expr().affine().constant());
}

void idol::Optimizers::Bilevel::PADM::update_mat_coeff(const idol::Ctr &t_ctr, const idol::Var &t_var) {
    m_single_level_reformulation.reset();
}

void idol::Optimizers::Bilevel::PADM::update_ctr_type(const idol::Ctr &t_ctr) {
    m_single_level_reformulation.reset();
}

void idol::Optimizers::Bilevel::PADM::update_ctr_rhs(const idol::Ctr &t_ctr) {
    throw_if_no_deterministic_model();
    m_single_level_reformulation->set_ctr_rhs(t_ctr, parent().get_ctr_rhs(t_ctr));
}

void idol::Optimizers::Bilevel::PADM::update_var_type(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_single_level_reformulation->set_var_type(t_var, parent().get_var_type(t_var));
}

void idol::Optimizers::Bilevel::PADM::update_var_lb(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_single_level_reformulation->set_var_lb(t_var, parent().get_var_lb(t_var));
}

void idol::Optimizers::Bilevel::PADM::update_var_ub(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_single_level_reformulation->set_var_ub(t_var, parent().get_var_ub(t_var));
}

void idol::Optimizers::Bilevel::PADM::update_var_obj(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_single_level_reformulation->set_var_obj(t_var, parent().get_var_obj(t_var));
}

void idol::Optimizers::Bilevel::PADM::throw_if_no_deterministic_model() const {
    if (!m_deterministic_optimizer) {
        throw Exception("Not available.");
    }
}

idol::SolutionStatus idol::Optimizers::Bilevel::PADM::get_status() const {
    if (!m_single_level_reformulation) {
        return Algorithm::get_status();
    }
    return m_single_level_reformulation->get_status();
}

idol::SolutionReason idol::Optimizers::Bilevel::PADM::get_reason() const {
    if (!m_single_level_reformulation) {
        return Algorithm::get_reason();
    }
    return m_single_level_reformulation->get_reason();
}

double idol::Optimizers::Bilevel::PADM::get_best_obj() const {
    if (!m_single_level_reformulation) {
        return Algorithm::get_best_obj();
    }
    return m_single_level_reformulation->get_best_obj();
}

double idol::Optimizers::Bilevel::PADM::get_best_bound() const {
    if (!m_single_level_reformulation) {
        return Algorithm::get_best_bound();
    }
    return m_single_level_reformulation->get_best_bound();
}
