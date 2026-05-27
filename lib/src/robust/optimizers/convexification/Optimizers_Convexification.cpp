//
// Created by Henri on 26/05/2026.
//
#include "idol/robust/optimizers/convexification/Optimizers_Convexification.h"
#include "idol/robust/modeling/Description.h"
#include "idol/bilevel/optimizers/KKT/KKT.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/IntegerMaster.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/Neame.h"
#include "idol/mixed-integer/optimizers/presolve/BoundRounding.h"
#include "idol/mixed-integer/optimizers/presolve/OneRowBoundTightening.h"

idol::Optimizers::Robust::Convexification::Convexification(const Model& t_parent,
                                                           const idol::Robust::Description& t_robust_description,
                                                           const idol::Bilevel::Description& t_bilevel_description,
                                                           const OptimizerFactory& t_master_optimizer,
                                                           const OptimizerFactory& t_sub_problem_optimizer,
                                                           const OptimizerFactory* t_integer_master_heuristic_optimizer)
        : Algorithm(t_parent),
          m_robust_description(t_robust_description),
          m_bilevel_description(t_bilevel_description),
          m_master_optimizer(t_master_optimizer.clone()),
          m_sub_problem_optimizer(t_sub_problem_optimizer.clone()),
          m_integer_master_heuristic_optimizer(t_integer_master_heuristic_optimizer ? t_integer_master_heuristic_optimizer->clone() : nullptr)
{

}

std::string idol::Optimizers::Robust::Convexification::name() const {
    return "Convexification";
}

double idol::Optimizers::Robust::Convexification::get_var_primal(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_var_primal(t_var);
}

double idol::Optimizers::Robust::Convexification::get_var_reduced_cost(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_var_reduced_cost(t_var);
}

double idol::Optimizers::Robust::Convexification::get_var_ray(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_var_ray(t_var);
}

double idol::Optimizers::Robust::Convexification::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_ctr_dual(t_ctr);
}

double idol::Optimizers::Robust::Convexification::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_ctr_farkas(t_ctr);
}

unsigned int idol::Optimizers::Robust::Convexification::get_n_solutions() const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_n_solutions();
}

unsigned int idol::Optimizers::Robust::Convexification::get_solution_index() const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_solution_index();
}

const idol::Model& idol::Optimizers::Robust::Convexification::deterministic_model() const {
    throw_if_no_deterministic_model();
    return *m_deterministic_model;
}

void idol::Optimizers::Robust::Convexification::add(const idol::Var &t_var) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Robust::Convexification::add(const idol::Ctr &t_ctr) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Robust::Convexification::add(const idol::QCtr &t_ctr) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Robust::Convexification::remove(const idol::Var &t_var) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Robust::Convexification::remove(const idol::Ctr &t_ctr) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Robust::Convexification::remove(const idol::QCtr &t_ctr) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Robust::Convexification::update() {
    m_deterministic_model.reset();
}

void idol::Optimizers::Robust::Convexification::write(const std::string &t_name) {
    throw_if_no_deterministic_model();
    m_deterministic_model->write(t_name);
}

void idol::Optimizers::Robust::Convexification::hook_optimize() {

    if (!m_deterministic_model) {

        build_deterministic_model();
        build_branch_and_price();

    }

    // Parameters
    m_deterministic_model->optimizer().set_param_logs(get_param_logs());
    m_deterministic_model->optimizer().set_param_time_limit(get_remaining_time());
    m_deterministic_model->optimizer().set_param_threads(get_param_thread_limit());
    m_deterministic_model->optimizer().set_param_best_bound_stop(get_param_best_bound_stop());
    m_deterministic_model->optimizer().set_param_best_obj_stop(get_param_best_obj_stop());
    m_deterministic_model->optimizer().set_param_presolve(get_param_presolve());
    m_deterministic_model->optimizer().set_param_infeasible_or_unbounded_info(get_param_infeasible_or_unbounded_info());

    // Tolerances
    m_deterministic_model->optimizer().set_tol_feasibility(get_tol_feasibility());
    m_deterministic_model->optimizer().set_tol_integer(get_tol_integer());
    m_deterministic_model->optimizer().set_tol_mip_absolute_gap(get_tol_mip_absolute_gap());
    m_deterministic_model->optimizer().set_tol_mip_relative_gap(get_tol_mip_relative_gap());
    m_deterministic_model->optimizer().set_tol_optimality(get_tol_optimality());

    m_deterministic_model->optimize();

}

void idol::Optimizers::Robust::Convexification::set_solution_index(unsigned int t_index) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_solution_index(t_index);
}

void idol::Optimizers::Robust::Convexification::update_obj_sense() {
    m_deterministic_model.reset();
}

void idol::Optimizers::Robust::Convexification::update_obj() {
    m_deterministic_model.reset();
}

void idol::Optimizers::Robust::Convexification::update_rhs() {
    m_deterministic_model.reset();
}

void idol::Optimizers::Robust::Convexification::update_obj_constant() {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_obj_const(parent().get_obj_expr().affine().constant());
}

void idol::Optimizers::Robust::Convexification::update_mat_coeff(const idol::Ctr &t_ctr, const idol::Var &t_var) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Robust::Convexification::update_ctr_type(const idol::Ctr &t_ctr) {
    m_deterministic_model.reset();
}

void idol::Optimizers::Robust::Convexification::update_ctr_rhs(const idol::Ctr &t_ctr) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_ctr_rhs(t_ctr, parent().get_ctr_rhs(t_ctr));
}

void idol::Optimizers::Robust::Convexification::update_var_type(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_type(t_var, parent().get_var_type(t_var));
}

void idol::Optimizers::Robust::Convexification::update_var_lb(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_lb(t_var, parent().get_var_lb(t_var));
}

void idol::Optimizers::Robust::Convexification::update_var_ub(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_ub(t_var, parent().get_var_ub(t_var));
}

void idol::Optimizers::Robust::Convexification::update_var_obj(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_obj(t_var, parent().get_var_obj(t_var));
}

void idol::Optimizers::Robust::Convexification::throw_if_no_deterministic_model() const {
    if (!m_deterministic_model) {
        throw Exception("Not available.");
    }
}

void idol::Optimizers::Robust::Convexification::build_deterministic_model() {

    const auto& parent = this->parent();
    auto& env = parent.env();

    m_first_stage_variables.clear();
    if (!m_decomposition) {
        m_decomposition = Annotation(env, "decomposition", MasterId);
    }

    m_deterministic_model = std::make_unique<Model>(parent.copy());

    // Create decomposition
    for (const auto& ctr : parent.ctrs()) {

        if (m_bilevel_description.is_upper(ctr)) {
            ctr.set(*m_decomposition, MasterId);
            continue;
        }

        const auto& row = parent.get_ctr_row(ctr);
        bool has_first_stage = false;
        for (const auto& [var, coeff] : row) {
            if (m_bilevel_description.is_upper(var)) {
                has_first_stage = true;
                assert(row.size() == 2);
                break;
            }
        }

        if (!has_first_stage) {
            ctr.set(*m_decomposition, 0);
        } else {
            ctr.set(*m_decomposition, MasterId);
        }

    }

    // Spot first-stage variables
    for (const auto& var : parent.vars()) {
        const auto& type = parent.get_var_type(var);
        if (m_bilevel_description.is_upper(var) && type != idol::Continuous) {
            m_first_stage_variables.push_back(var);
        }
    }

    // Dualize Uncertain part of the objecitve
    idol::QuadExpr objective_function;
    for (const auto& [var, coeff] : m_robust_description.uncertain_obj()) {
        objective_function -= var * coeff;
    }

    idol::Reformulators::KKT reformulator(
        m_robust_description.uncertainty_set(),
        objective_function,
        [this](const auto& t_var) { return m_robust_description.uncertainty_set().has(t_var); }
    );

    m_deterministic_model->set_obj_expr(0.);
    reformulator.add_dual(*m_deterministic_model);
    m_deterministic_model->set_obj_sense(idol::Minimize);
    m_deterministic_model->set_obj_expr(parent.get_obj_expr() - m_deterministic_model->get_obj_expr());

}

void idol::Optimizers::Robust::Convexification::build_branch_and_price() {

    auto branch_and_bound = idol::BranchAndBound();

    branch_and_bound.with_branching_rule(idol::PseudoCost(m_first_stage_variables.begin(), m_first_stage_variables.end()));
    branch_and_bound.with_node_selection_rule(idol::BestBound());
    if (m_integer_master_heuristic_optimizer) {
        branch_and_bound.add_callback(idol::Heuristics::IntegerMaster().with_optimizer(*m_integer_master_heuristic_optimizer).with_integer_columns(false));
    }
    branch_and_bound.add_presolver(idol::Presolvers::BoundRounding());
    branch_and_bound.add_presolver(idol::Presolvers::OneRowBoundTightening());

    const auto subproblem_specifications = idol::DantzigWolfe::SubProblem()
            .add_optimizer(*m_sub_problem_optimizer)
            .with_max_column_per_pricing(10)
            .with_column_pool_clean_up(1500, .75)
    ;

    auto column_generation = idol::DantzigWolfeDecomposition(*m_decomposition);

    column_generation.with_default_sub_problem_spec(subproblem_specifications);
    column_generation.with_master_optimizer(*m_master_optimizer);
    column_generation.with_dual_price_smoothing_stabilization(idol::DantzigWolfe::Neame(.2));
    column_generation.with_infeasibility_strategy(idol::DantzigWolfe::FarkasPricing());

    m_deterministic_model->use(branch_and_bound + column_generation);

}

idol::SolutionStatus idol::Optimizers::Robust::Convexification::get_status() const {
    if (!m_deterministic_model) {
        return Algorithm::get_status();
    }
    return m_deterministic_model->get_status();
}

idol::SolutionReason idol::Optimizers::Robust::Convexification::get_reason() const {
    if (!m_deterministic_model) {
        return Algorithm::get_reason();
    }
    return m_deterministic_model->get_reason();
}

double idol::Optimizers::Robust::Convexification::get_best_obj() const {
    if (!m_deterministic_model) {
        return Algorithm::get_best_obj();
    }
    return m_deterministic_model->get_best_obj();
}

double idol::Optimizers::Robust::Convexification::get_best_bound() const {
    if (!m_deterministic_model) {
        return Algorithm::get_best_bound();
    }
    return m_deterministic_model->get_best_bound();
}
