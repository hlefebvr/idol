//
// Created by henri on 29.11.24.
//
#include "idol/bilevel/optimizers/MinMaxDualize/Optimizers_MinMax_Dualize.h"
#include "idol/bilevel/optimizers/MinMaxDualize/MinMax_Dualize.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

idol::Optimizers::Bilevel::MinMax::Dualize::Dualize(const Model& t_parent,
                                                       const idol::Bilevel::Description &t_description,
                                                       const OptimizerFactory &t_deterministic_optimizer,
                                                       const std::unique_ptr<Reformulators::KKT::BoundProvider>& t_bound_provider)
        : Algorithm(t_parent),
          m_description(t_description),
          m_deterministic_optimizer(t_deterministic_optimizer.clone()),
          m_bound_provider(t_bound_provider ? t_bound_provider->clone() : nullptr) {

}

std::string idol::Optimizers::Bilevel::MinMax::Dualize::name() const {
    return "min-max-dualize";
}

double idol::Optimizers::Bilevel::MinMax::Dualize::get_var_primal(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    if (m_lower_level_model && m_lower_level_model->has(t_var)) {
        return m_lower_level_model->get_var_primal(t_var);
    }
    return m_deterministic_model->get_var_primal(t_var);
}

double idol::Optimizers::Bilevel::MinMax::Dualize::get_var_reduced_cost(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    if (m_lower_level_model && m_lower_level_model->has(t_var)) {
        return m_lower_level_model->get_var_reduced_cost(t_var);
    }
    return m_deterministic_model->get_var_reduced_cost(t_var);
}

double idol::Optimizers::Bilevel::MinMax::Dualize::get_var_ray(const idol::Var &t_var) const {
    throw_if_no_deterministic_model();
    if (m_lower_level_model && m_lower_level_model->has(t_var)) {
        return m_lower_level_model->get_var_ray(t_var);
    }
    return m_deterministic_model->get_var_ray(t_var);
}

double idol::Optimizers::Bilevel::MinMax::Dualize::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw_if_no_deterministic_model();
    if (m_lower_level_model && m_lower_level_model->has(t_ctr)) {
        return m_lower_level_model->get_ctr_dual(t_ctr);
    }
    return m_deterministic_model->get_ctr_dual(t_ctr);
}

double idol::Optimizers::Bilevel::MinMax::Dualize::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw_if_no_deterministic_model();
    if (m_lower_level_model && m_lower_level_model->has(t_ctr)) {
        return m_lower_level_model->get_ctr_farkas(t_ctr);
    }
    return m_deterministic_model->get_ctr_farkas(t_ctr);
}

unsigned int idol::Optimizers::Bilevel::MinMax::Dualize::get_n_solutions() const {
    throw_if_no_deterministic_model();
    return 1;
}

unsigned int idol::Optimizers::Bilevel::MinMax::Dualize::get_solution_index() const {
    throw_if_no_deterministic_model();
    return m_deterministic_model->get_solution_index();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::add(const idol::Var &t_var) {
    m_deterministic_model.reset();
    m_lower_level_model.reset();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::add(const idol::Ctr &t_ctr) {
    m_deterministic_model.reset();
    m_lower_level_model.reset();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::add(const idol::QCtr &t_ctr) {
    m_deterministic_model.reset();
    m_lower_level_model.reset();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::remove(const idol::Var &t_var) {
    m_deterministic_model.reset();
    m_lower_level_model.reset();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::remove(const idol::Ctr &t_ctr) {
    m_deterministic_model.reset();
    m_lower_level_model.reset();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::remove(const idol::QCtr &t_ctr) {
    m_deterministic_model.reset();
    m_lower_level_model.reset();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::update() {
    m_deterministic_model.reset();
    m_lower_level_model.reset();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::write(const std::string &t_name) {
    create_deterministic_model_if_not_exists();
    m_deterministic_model->write(t_name);
}

void idol::Optimizers::Bilevel::MinMax::Dualize::hook_optimize() {

    create_deterministic_model_if_not_exists();

    m_deterministic_model->optimize();

    if (m_skip_solving_lower_level) {
        return;
    }

    if (get_status() == Optimal || get_status() == Feasible) {
        solve_lower_level();
    }

}

void idol::Optimizers::Bilevel::MinMax::Dualize::set_solution_index(unsigned int t_index) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_solution_index(t_index);
}

void idol::Optimizers::Bilevel::MinMax::Dualize::update_obj_sense() {
    m_deterministic_model.reset();
    m_lower_level_model.reset();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::update_obj() {
    m_deterministic_model.reset();
    m_lower_level_model.reset();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::update_rhs() {
    m_deterministic_model.reset();
    m_lower_level_model.reset();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::update_obj_constant() {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_obj_const(parent().get_obj_expr().affine().constant());
}

void idol::Optimizers::Bilevel::MinMax::Dualize::update_mat_coeff(const idol::Ctr &t_ctr, const idol::Var &t_var) {
    m_deterministic_model.reset();
    m_lower_level_model.reset();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::update_ctr_type(const idol::Ctr &t_ctr) {
    m_deterministic_model.reset();
    m_lower_level_model.reset();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::update_ctr_rhs(const idol::Ctr &t_ctr) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_ctr_rhs(t_ctr, parent().get_ctr_rhs(t_ctr));
}

void idol::Optimizers::Bilevel::MinMax::Dualize::update_var_type(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_type(t_var, parent().get_var_type(t_var));
}

void idol::Optimizers::Bilevel::MinMax::Dualize::update_var_lb(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_lb(t_var, parent().get_var_lb(t_var));
}

void idol::Optimizers::Bilevel::MinMax::Dualize::update_var_ub(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_ub(t_var, parent().get_var_ub(t_var));
}

void idol::Optimizers::Bilevel::MinMax::Dualize::update_var_obj(const idol::Var &t_var) {
    throw_if_no_deterministic_model();
    m_deterministic_model->set_var_obj(t_var, parent().get_var_obj(t_var));
}

void idol::Optimizers::Bilevel::MinMax::Dualize::throw_if_no_deterministic_model() const {
    if (!m_deterministic_optimizer) {
        throw Exception("Not available.");
    }
}

idol::SolutionStatus idol::Optimizers::Bilevel::MinMax::Dualize::get_status() const {
    if (!m_deterministic_model) {
        return Algorithm::get_status();
    }
    return m_deterministic_model->get_status();
}

idol::SolutionReason idol::Optimizers::Bilevel::MinMax::Dualize::get_reason() const {
    if (!m_deterministic_model) {
        return Algorithm::get_reason();
    }
    return m_deterministic_model->get_reason();
}

double idol::Optimizers::Bilevel::MinMax::Dualize::get_best_obj() const {
    if (!m_deterministic_model) {
        return Algorithm::get_best_obj();
    }
    return m_deterministic_model->get_best_obj();
}

double idol::Optimizers::Bilevel::MinMax::Dualize::get_best_bound() const {
    if (!m_deterministic_model) {
        return Algorithm::get_best_bound();
    }
    return m_deterministic_model->get_best_bound();
}

void idol::Optimizers::Bilevel::MinMax::Dualize::solve_lower_level() {

    const auto& parent = this->parent();
    auto& env = parent.env();
    m_lower_level_model = std::make_unique<Model>(env);

    for (const auto& var : parent.vars()) {

        if (m_description.is_upper(var)) {
            continue;
        }

        const double lb = parent.get_var_lb(var);
        const double ub = parent.get_var_ub(var);
        const auto type = parent.get_var_type(var);
        m_lower_level_model->add(var, TempVar(lb, ub, type, 0., LinExpr<Ctr>()));

    }

    for (const auto& ctr : parent.ctrs()) {

        LinExpr<Var> lhs;
        double rhs = parent.get_ctr_rhs(ctr);

        for (const auto& [var, coeff] : parent.get_ctr_row(ctr)) {
            if (m_description.is_upper(var)) {
                rhs -= coeff * m_deterministic_model->get_var_primal(var);
                continue;
            }
            lhs += coeff * var;
        }

        m_lower_level_model->add(ctr, TempCtr(std::move(lhs), parent.get_ctr_type(ctr), rhs));

    }

    m_lower_level_model->set_obj_expr(m_description.lower_level_obj());

    if (parent.qctrs().size() > 0) {
        throw Exception("Quadratic constraints are not supported.");
    }

    m_lower_level_model->use(*m_deterministic_optimizer);
    m_lower_level_model->optimizer().set_param_time_limit(get_remaining_time());
    m_lower_level_model->optimize();

    if (m_lower_level_model->get_status() != Optimal) {
        set_status(Fail);
        set_reason(NotSpecified);
        return;
    }

}

void idol::Optimizers::Bilevel::MinMax::Dualize::create_deterministic_model_if_not_exists() {

    if (!m_deterministic_model) {
        if (m_bound_provider) {
            m_deterministic_model = std::make_unique<Model>(idol::Bilevel::MinMax::Dualize::make_model(parent(), m_description, *m_bound_provider));
        } else {
            m_deterministic_model = std::make_unique<Model>(idol::Bilevel::MinMax::Dualize::make_model(parent(), m_description));
        }
        m_deterministic_model->use(*m_deterministic_optimizer);
    }

}
