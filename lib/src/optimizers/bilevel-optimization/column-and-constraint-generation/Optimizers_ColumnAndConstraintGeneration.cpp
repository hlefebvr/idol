//
// Created by henri on 04.03.24.
//

#include <cassert>
#include "idol/optimizers/bilevel-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/optimizers/mixed-integer-programming/wrappers/Gurobi/Optimizers_Gurobi.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/separators/Dualize.h"
#include "idol/modeling/expressions/operations/operators.h"
#include "idol/modeling/objects/Versions.h"

class ExtendedLowerLevelSeparator : public idol::Robust::ColumnAndConstraintGenerationSeparator {
    const idol::Bilevel::impl::MinMaxMinFormulation& m_formulation;
    mutable idol::Model m_extended_model;
public:
    ExtendedLowerLevelSeparator(const idol::Bilevel::impl::MinMaxMinFormulation& t_formulation,
                                const idol::OptimizerFactory &t_optimizer_factory);

    ExtendedLowerLevelSeparator(const ExtendedLowerLevelSeparator& t_src);

    idol::Solution::Primal operator()(const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                const idol::Solution::Primal &t_upper_level_solution,
                                const idol::Row& t_row,
                                      idol::CtrType t_type) const override;

    [[nodiscard]] ColumnAndConstraintGenerationSeparator *clone() const override;
};

ExtendedLowerLevelSeparator::ExtendedLowerLevelSeparator(const idol::Bilevel::impl::MinMaxMinFormulation& t_formulation,
                                                         const idol::OptimizerFactory &t_optimizer_factory)
                                                         : m_formulation(t_formulation),
                                                           m_extended_model(t_formulation.extended_lower_level().copy()) {

    m_extended_model.set_obj_expr(idol::Expr());
    m_extended_model.use(t_optimizer_factory);

}

idol::Solution::Primal
ExtendedLowerLevelSeparator::operator()(const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                        const idol::Solution::Primal &t_upper_level_solution,
                                        const idol::Row &t_row,
                                        idol::CtrType t_type) const {

    auto objective = m_formulation.extended_lower_level().get_obj_expr().fix(t_upper_level_solution);

    m_extended_model.set_obj_expr(objective);

    m_extended_model.optimize();

    const auto status = m_extended_model.get_status();

    if (status != idol::Optimal) {
        idol::Solution::Primal result;
        result.set_status(status);
        result.set_reason(m_extended_model.get_reason());
        return result;
    }

    auto result = idol::save_primal(m_formulation.uncertainty_set(), m_extended_model);

    // Compute current follower objective, TODO: move to a function
    const auto follower_objective = m_formulation.high_point_relaxation().get_ctr_row(m_formulation.parent().lower_level_objective());
    const auto expr = follower_objective.linear() - follower_objective.rhs();
    const double val = idol::evaluate(expr, t_upper_level_solution);

    result.set_objective_value(val - result.objective_value());

    return result;

}

idol::Robust::ColumnAndConstraintGenerationSeparator *ExtendedLowerLevelSeparator::clone() const {
    return new ExtendedLowerLevelSeparator(*this);
}

ExtendedLowerLevelSeparator::ExtendedLowerLevelSeparator(const ExtendedLowerLevelSeparator &t_src)
    :
    m_formulation(t_src.m_formulation),
    m_extended_model(t_src.m_extended_model.copy()) {

}

idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(const idol::Model &t_parent,
                                                                                        const idol::Annotation<idol::Var, unsigned int> &t_lower_level_variables,
                                                                                        const idol::Annotation<idol::Ctr, unsigned int> &t_lower_level_constraints,
                                                                                        idol::Ctr t_lower_level_objective,
                                                                                        const idol::OptimizerFactory &t_master_optimizer,
                                                                                        const idol::OptimizerFactory &t_lower_level_optimizer)
    : idol::Algorithm(t_parent),
      m_lower_level_objective(std::move(t_lower_level_objective)),
      m_lower_level_variables(t_lower_level_variables),
      m_lower_level_constraints(t_lower_level_constraints),
      m_master_optimizer(t_master_optimizer.clone()),
      m_lower_level_optimizer(t_lower_level_optimizer.clone()),
      m_variable_stage(std::make_optional<Annotation<Var, unsigned int>>(t_parent.env(), "variable_stage", MasterId)),
      m_constraint_stage(std::make_optional<Annotation<Ctr, unsigned int>>(t_parent.env(), "constraint_stage", MasterId)) {

}

std::string idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::name() const {
    return "bilevel-column-and-constraint-generation";
}

double idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_var_primal(const idol::Var &t_var) const {
    if (!m_two_stage_robust_model) {
        throw Exception("Not available.");
    }
    return m_two_stage_robust_model->get_var_primal(t_var);
}

double idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_var_reduced_cost(const idol::Var &t_var) const {
    if (!m_two_stage_robust_model) {
        throw Exception("Not available.");
    }
    return m_two_stage_robust_model->get_var_reduced_cost(t_var);
}

double idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_var_ray(const idol::Var &t_var) const {
    if (!m_two_stage_robust_model) {
        throw Exception("Not available.");
    }
    return m_two_stage_robust_model->get_var_ray(t_var);
}

double idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_ctr_dual(const idol::Ctr &t_ctr) const {
    if (!m_two_stage_robust_model) {
        throw Exception("Not available.");
    }
    return m_two_stage_robust_model->get_ctr_dual(t_ctr);
}

double idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    if (!m_two_stage_robust_model) {
        throw Exception("Not available.");
    }
    return m_two_stage_robust_model->get_ctr_farkas(t_ctr);
}

unsigned int idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_n_solutions() const {
    if (!m_two_stage_robust_model) {
        return 0;
    }
    return m_two_stage_robust_model->get_n_solutions();
}

unsigned int idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::get_solution_index() const {
    return 0;
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::add(const idol::Var &t_var) {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::add(const idol::Ctr &t_ctr) {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::remove(const idol::Var &t_var) {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::remove(const idol::Ctr &t_ctr) {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update() {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::write(const std::string &t_name) {
    throw Exception("Not implemented write");
}

double idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::compute_penalty_parameter() const {

    const auto& parent = this->parent();

    double result = 0;

    for (const auto& var : parent.vars()) {

        if (var.get(m_lower_level_variables) == MasterId) {
            continue;
        }

        const double lb = parent.get_var_lb(var);
        const double ub = parent.get_var_ub(var);
        const double obj = parent.get_var_column(var).obj().as_numerical();

        result += std::abs(obj) * (ub - lb);

    }

    return result;
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::hook_before_optimize() {
    Optimizer::hook_before_optimize();

    assert(m_variable_stage.has_value());
    assert(m_constraint_stage.has_value());

    const double penalty = compute_penalty_parameter();

    m_formulation = std::make_unique<idol::Bilevel::impl::MinMaxMinFormulation>(*this,
                                                                                *m_variable_stage,
                                                                                *m_constraint_stage,
                                                                                penalty);

    m_two_stage_robust_model = std::make_unique<Model>(m_formulation->two_stage_robust_formulation().copy());

    if (m_use_extended_level_separator) {

        m_separator = std::make_unique<ExtendedLowerLevelSeparator>(*m_formulation, *m_lower_level_optimizer);

    } else {

        m_separator.reset(idol::Robust::ColumnAndConstraintSeparators::Dualize()
                                .with_optimizer(*m_lower_level_optimizer)
                                .clone()
        );

    }

    m_two_stage_robust_model->use(
            idol::Robust::ColumnAndConstraintGeneration(*m_variable_stage,
                                                        *m_constraint_stage,
                                                        m_formulation->uncertainty_set())
                    .with_master_optimizer(*m_master_optimizer)
                    .with_separator(*m_separator)
                    .with_complete_recourse(true)
                    .with_logs(get_param_logs())
                    .with_iteration_limit(get_param_iteration_limit())
                    .with_time_limit(get_param_time_limit())
                    .with_thread_limit(get_param_thread_limit())
                    .with_presolve(get_param_presolve())
                    .with_infeasible_or_unbounded_info(get_param_infeasible_or_unbounded_info())
                    .with_best_bound_stop(get_param_best_bound_stop())
                    .with_best_obj_stop(get_param_best_obj_stop())
    );

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::hook_optimize() {

    m_two_stage_robust_model->optimize();

    set_status(m_two_stage_robust_model->get_status());
    set_reason(m_two_stage_robust_model->get_reason());
    set_best_obj(m_two_stage_robust_model->get_best_obj());
    set_best_bound(m_two_stage_robust_model->get_best_bound());

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::set_solution_index(unsigned int t_index) {
    if (!m_two_stage_robust_model) {
        throw Exception("Cannot set solution index before optimize is called.");
    }
    m_two_stage_robust_model->set_solution_index(t_index);
}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_obj_sense() {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_obj() {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_rhs() {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_obj_constant() {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_mat_coeff(const idol::Ctr &t_ctr,
                                                                                const idol::Var &t_var) {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_ctr_type(const idol::Ctr &t_ctr) {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_ctr_rhs(const idol::Ctr &t_ctr) {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_var_type(const idol::Var &t_var) {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_var_lb(const idol::Var &t_var) {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_var_ub(const idol::Var &t_var) {

}

void idol::Optimizers::Bilevel::ColumnAndConstraintGeneration::update_var_obj(const idol::Var &t_var) {

}
