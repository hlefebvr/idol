//
// Created by henri on 21.11.24.
//

#include "idol/mixed-integer/modeling/models/KKT.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/modeling/constraints/TempQCtr.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"
#include "idol/bilevel/modeling/Description.h"
#include <cassert>

idol::Reformulators::KKT::KKT(const idol::Model &t_parent,
                         const QuadExpr<Var, double> &t_primal_objective,
                         const std::function<bool(const Var &)> &t_primal_variable_indicator,
                         const std::function<bool(const Ctr &)> &t_primal_constraint_indicator,
                         const std::function<bool(const QCtr &)> &t_primal_qconstraint_indicator)
                         : m_primal(t_parent),
                           m_primal_objective(t_primal_objective),
                           m_primal_variable_indicator(t_primal_variable_indicator),
                           m_primal_constraint_indicator(t_primal_constraint_indicator),
                           m_primal_qconstraint_indicator(t_primal_qconstraint_indicator) {

    if (!primal_is_a_linear_problem()) {
        throw Exception("KKT only supports linear problems.");
    }

    create_dual_variables();
    create_dual_constraints();

}

idol::Reformulators::KKT::KKT(const idol::Model &t_parent,
                         const std::function<bool(const Var &)> &t_primal_variable_indicator,
                         const std::function<bool(const Ctr &)> &t_primal_constraint_indicator,
                         const std::function<bool(const QCtr &)> &t_primal_qconstraint_indicator)
                         : KKT(t_parent,
                                    t_parent.get_obj_expr(),
                                    t_primal_variable_indicator,
                                    t_primal_constraint_indicator,
                                    t_primal_qconstraint_indicator) {

}

idol::Reformulators::KKT::KKT(const Model& t_high_point_relaxation,
                         const Bilevel::Description &t_bilevel_description)
                         : KKT(t_high_point_relaxation,
                                    t_bilevel_description.follower_obj(),
                                    [&](const Var& t_var) { return t_bilevel_description.is_follower(t_var); },
                                    [&](const Ctr& t_ctr) { return t_bilevel_description.is_follower(t_ctr); },
                                    [&](const QCtr& t_qctr) { return t_bilevel_description.is_follower(t_qctr); }) {

}

std::pair<double, double> idol::Reformulators::KKT::bounds_for_dual_variable(idol::CtrType t_type) const {
    switch (t_type) {
        case LessOrEqual: return { -Inf, 0 };
        case Equal: return { -Inf, Inf };
        case GreaterOrEqual: return { 0, Inf };
        default: throw Exception("Invalid constraint type.");
    }
}

void idol::Reformulators::KKT::create_dual_variables() {

    auto& env = m_primal.env();

    // Create dual variables for primal constraints
    m_dual_variables_for_constraints.resize(m_primal.ctrs().size());

    for (const auto& ctr : m_primal.ctrs()) {

        if (!m_primal_constraint_indicator(ctr)) {
            continue;
        }

        const unsigned int index = m_primal.get_ctr_index(ctr);
        const auto type = m_primal.get_ctr_type(ctr);

        const auto [lb, ub] = bounds_for_dual_variable(type);

        m_dual_variables_for_constraints[index] = Var(env,  lb, ub, Continuous, 0, m_prefix + "dual_" + ctr.name());

    }

    // Create dual variables for primal quadratic constraints
    m_dual_variables_for_qconstraints.resize(m_primal.qctrs().size());

    for (const auto& qctr : m_primal.qctrs()) {

        if (!m_primal_qconstraint_indicator(qctr)) {
            continue;
        }

        const unsigned int index = m_primal.get_qctr_index(qctr);
        const auto type = m_primal.get_qctr_type(qctr);

        const auto [lb, ub] = bounds_for_dual_variable(type);

        m_dual_variables_for_qconstraints[index] = Var(env, lb, ub, Continuous, 0, m_prefix + "dual_" + qctr.name());

    }

    // Create dual variables for primal variable lower bounds
    m_dual_variables_for_lower_bounds.resize(m_primal.vars().size());

    for (const auto& var : m_primal.vars()) {

        if (!m_primal_variable_indicator(var)) {
            continue;
        }

        const auto lb = m_primal.get_var_lb(var);
        const unsigned int index = m_primal.get_var_index(var);

        if (is_neg_inf(lb)) {
            continue;
        }

        m_dual_variables_for_lower_bounds[index] = Var(env, 0, Inf, Continuous, 0, m_prefix + "dual_lb_" + var.name());

    }

    // Create dual variables for primal variable upper bounds
    m_dual_variables_for_upper_bounds.resize(m_primal.vars().size());

    for (const auto& var : m_primal.vars()) {

        if (!m_primal_variable_indicator(var)) {
            continue;
        }

        const auto ub = m_primal.get_var_ub(var);
        const unsigned int index = m_primal.get_var_index(var);

        if (is_pos_inf(ub)) {
            continue;
        }

        m_dual_variables_for_upper_bounds[index] = Var(env, -Inf, 0, Continuous, 0, "dual_ub_" + var.name());

    }

}

bool idol::Reformulators::KKT::primal_is_a_linear_problem() const {

    if (!is_linear(m_primal_objective)) {
        return false;
    }

    for (const auto& qctr : m_primal.qctrs()) {

        if (!m_primal_qconstraint_indicator(qctr)) {
            continue;
        }

        if (is_linear(m_primal.get_qctr_expr(qctr))) {
            continue;
        }

        return false;
    }

    for (const auto& var : m_primal.vars()) {

        if (!m_primal_variable_indicator(var)) {
            continue;
        }

        const auto type = m_primal.get_var_type(var);

        if (type == Continuous) {
            continue;
        }

        return false;
    }

    return true;
}

bool idol::Reformulators::KKT::is_linear(const QuadExpr<Var, double>& t_expr) const {

    for (const auto& [pair, constant] : t_expr) {

        if (m_primal_variable_indicator(pair.first) && m_primal_variable_indicator(pair.second)) {
            return false;
        }

    }

    return true;
}

void idol::Reformulators::KKT::create_dual_constraints() {

    auto& env = m_primal.env();

    m_dual_objective = m_primal_objective.affine().constant();

    std::vector<QuadExpr<Var, double>> dual_constraint_expressions;
    dual_constraint_expressions.resize(m_primal.vars().size());

    const auto add_participation_of_row = [&](const Var& t_dual_variable, const LinExpr<Var>& t_row) {

        for (const auto& [var, coefficient] : t_row) {

            if (!m_primal_variable_indicator(var)) {
                m_dual_objective += -coefficient * (var * t_dual_variable);
                continue;
            }

            const unsigned int var_index = m_primal.get_var_index(var);
            dual_constraint_expressions[var_index] += coefficient * t_dual_variable;

        }

    };

    // Add participation of primal constraints
    for (const auto& ctr : m_primal.ctrs()) {

        if (!m_primal_constraint_indicator(ctr)) {
            continue;
        }

        const unsigned int index = m_primal.get_ctr_index(ctr);
        const auto& row = m_primal.get_ctr_row(ctr);
        const double rhs = m_primal.get_ctr_rhs(ctr);
        const auto& dual_variable = *m_dual_variables_for_constraints[index];

        m_dual_objective += rhs * dual_variable;
        add_participation_of_row(dual_variable, row);
    }

    // Add participation of lower and upper bounds
    for (const auto& var : m_primal.vars()) {

        if (!m_primal_variable_indicator(var)) {
            continue;
        }

        const unsigned int index = m_primal.get_var_index(var);

        const auto lb = m_primal.get_var_lb(var);
        const auto ub = m_primal.get_var_ub(var);

        if (!is_neg_inf(lb)) {
            dual_constraint_expressions[index] += *m_dual_variables_for_lower_bounds[index];
            m_dual_objective += lb * *m_dual_variables_for_lower_bounds[index];
        }

        if (!is_pos_inf(ub)) {
            dual_constraint_expressions[index] += *m_dual_variables_for_upper_bounds[index];
            m_dual_objective += ub * *m_dual_variables_for_upper_bounds[index];
        }

    }

    // Add participation of primal quadratic constraints
    for (const auto& qctr : m_primal.qctrs()) {

        if (!m_primal_qconstraint_indicator(qctr)) {
            continue;
        }

        const unsigned int index = m_primal.get_qctr_index(qctr);
        const auto& expr = m_primal.get_qctr_expr(qctr);
        const auto& dual_variable = *m_dual_variables_for_qconstraints[index];

        m_dual_objective += -expr.affine().constant() * dual_variable;
        add_participation_of_row(dual_variable, expr.affine().linear());

        for (const auto& [pair, coefficient] : expr) {

            if (m_primal_variable_indicator(pair.first)) {
                const unsigned int var_index = m_primal.get_var_index(pair.first);
                dual_constraint_expressions[var_index] += coefficient * (pair.second * dual_variable);
                continue;
            }

            if (m_primal_variable_indicator(pair.second)) {
                const unsigned int var_index = m_primal.get_var_index(pair.second);
                dual_constraint_expressions[var_index] += coefficient * (pair.first * dual_variable);
                continue;
            }

            assert(false);

        }

    }

    // Add participation of primal objective
    for (const auto& [var, coefficient] : m_primal_objective.affine().linear()) {

        if (!m_primal_variable_indicator(var)) {
            m_dual_objective += coefficient * var;
            continue;
        }

        const unsigned int var_index = m_primal.get_var_index(var);
        dual_constraint_expressions[var_index] += -coefficient;

    }
    for (const auto& [pair, coefficient] : m_primal_objective) {

        if (m_primal_variable_indicator(pair.first)) {
            const unsigned int var_index = m_primal.get_var_index(pair.first);
            dual_constraint_expressions[var_index] += -coefficient * pair.second;
            continue;
        }

        if (m_primal_variable_indicator(pair.second)) {
            const unsigned int var_index = m_primal.get_var_index(pair.second);
            dual_constraint_expressions[var_index] += -coefficient * pair.first;
            continue;
        }

        m_dual_objective += coefficient * (pair.first * pair.second);

    }

    m_dual_constraints.resize(m_primal.vars().size());
    for (const auto& var : m_primal.vars()) {

        if (!m_primal_variable_indicator(var)) {
            continue;
        }

        const auto index = m_primal.get_var_index(var);
        auto& dual_constraint_expression = dual_constraint_expressions[index];

        if (!dual_constraint_expression.has_quadratic()) {
            m_dual_constraints[index] = Ctr(env,
                                            TempCtr(
                                                    std::move(dual_constraint_expression.affine().linear()),
                                                    Equal,
                                                    -dual_constraint_expression.affine().constant()),
                                            "dual_" + var.name());
        } else {
            m_dual_constraints[index] = QCtr(env,
                                             TempQCtr(
                                                     std::move(dual_constraint_expression),
                                                     Equal),
                                             "dual_" + var.name());
        }

    }

}

void idol::Reformulators::KKT::add_dual(idol::Model &t_destination, bool t_add_objective) {

    add_dual_variables(t_destination);
    add_dual_constraints(t_destination);

    if (t_add_objective) {
        add_dual_objective(t_destination);
    }

}

void idol::Reformulators::KKT::add_dual_variables(idol::Model &t_destination) {

    for (const auto& opt_dual_var : m_dual_variables_for_constraints) {
        if (opt_dual_var.has_value()) {
            t_destination.add(*opt_dual_var);
        }
    }

    for (const auto& opt_dual_var : m_dual_variables_for_qconstraints) {
        if (opt_dual_var.has_value()) {
            t_destination.add(*opt_dual_var);
        }
    }

    for (const auto& opt_dual_var : m_dual_variables_for_lower_bounds) {
        if (opt_dual_var.has_value()) {
            t_destination.add(*opt_dual_var);
        }
    }

    for (const auto& opt_dual_var : m_dual_variables_for_upper_bounds) {
        if (opt_dual_var.has_value()) {
            t_destination.add(*opt_dual_var);
        }
    }

}

void idol::Reformulators::KKT::add_dual_constraints(idol::Model &t_destination) {

    for (const auto& opt_dual_ctr : m_dual_constraints) {
        if (opt_dual_ctr.has_value()) {
            if (std::holds_alternative<Ctr>(*opt_dual_ctr)) {
                t_destination.add(std::get<Ctr>(*opt_dual_ctr));
            } else {
                t_destination.add(std::get<QCtr>(*opt_dual_ctr));
            }
        }
    }

}

void idol::Reformulators::KKT::add_dual_objective(idol::Model &t_destination) {
    t_destination.set_obj_expr(t_destination.get_obj_expr() + m_dual_objective);
    t_destination.set_obj_sense(Maximize);
}

void idol::Reformulators::KKT::add_coupling(idol::Model &t_destination) {
    add_coupling_variables(t_destination);
    add_coupling_constraints(t_destination);
}

void idol::Reformulators::KKT::add_coupling_variables(idol::Model &t_destination) {

    for (const auto& var : m_primal.vars()) {

        if (m_primal_variable_indicator(var)) {
            continue;
        }

        const double lb = m_primal.get_var_lb(var);
        const double ub = m_primal.get_var_ub(var);
        const auto type = m_primal.get_var_type(var);

        t_destination.add(var, TempVar(lb, ub, type, 0, LinExpr<Ctr>()));

    }

}

void idol::Reformulators::KKT::add_coupling_constraints(idol::Model &t_destination) {

    for (const auto& ctr : m_primal.ctrs()) {

        if (m_primal_constraint_indicator(ctr)) {
            continue;
        }

        const auto& row = m_primal.get_ctr_row(ctr);
        const double rhs = m_primal.get_ctr_rhs(ctr);
        const auto type = m_primal.get_ctr_type(ctr);

        t_destination.add(ctr, TempCtr(LinExpr(row), type, rhs));

    }

    for (const auto& qctr : m_primal.qctrs()) {

        if (m_primal_qconstraint_indicator(qctr)) {
            continue;
        }

        const auto& expr = m_primal.get_qctr_expr(qctr);
        const auto type = m_primal.get_qctr_type(qctr);

        t_destination.add(qctr, TempQCtr(QuadExpr(expr), type));

    }

}

void idol::Reformulators::KKT::add_primal(idol::Model &t_destination, bool t_add_objective) {

    add_primal_variables(t_destination);
    add_primal_constraints(t_destination);

    if (t_add_objective) {
        add_primal_objective(t_destination);
    }

}

void idol::Reformulators::KKT::add_primal_variables(idol::Model &t_destination) {
    for (const auto& var : m_primal.vars()) {

        if (!m_primal_variable_indicator(var)) {
            continue;
        }

        const double lb = m_primal.get_var_lb(var);
        const double ub = m_primal.get_var_ub(var);
        const auto type = m_primal.get_var_type(var);

        t_destination.add(var, TempVar(lb, ub, type, 0, LinExpr<Ctr>()));
    }
}

void idol::Reformulators::KKT::add_primal_constraints(idol::Model &t_destination) {

    for (const auto& ctr : m_primal.ctrs()) {

        if (!m_primal_constraint_indicator(ctr)) {
            continue;
        }

        const auto& row = m_primal.get_ctr_row(ctr);
        const double rhs = m_primal.get_ctr_rhs(ctr);
        const auto type = m_primal.get_ctr_type(ctr);

        t_destination.add(ctr, TempCtr(LinExpr(row), type, rhs));
    }

    for (const auto& qctr : m_primal.qctrs()) {

        if (!m_primal_qconstraint_indicator(qctr)) {
            continue;
        }

        const auto& expr = m_primal.get_qctr_expr(qctr);
        const auto type = m_primal.get_qctr_type(qctr);

        t_destination.add(qctr, TempQCtr(QuadExpr(expr), type));
    }
}

void idol::Reformulators::KKT::add_primal_objective(idol::Model &t_destination) {
    t_destination.set_obj_expr(m_primal_objective);
    t_destination.set_obj_sense(Minimize);
}

void idol::Reformulators::KKT::add_strong_duality_reformulation(idol::Model &t_destination) {

    add_primal_variables(t_destination);
    add_dual_variables(t_destination);

    add_primal_constraints(t_destination);
    add_dual_constraints(t_destination);

    auto duality_gap = m_primal_objective - m_dual_objective;

    if (duality_gap.has_quadratic()) {
        t_destination.add_qctr(std::move(duality_gap), LessOrEqual, "strong_duality");
    } else {
        t_destination.add_ctr(std::move(duality_gap.affine()) <= 0, "strong_duality");
    }
}

void idol::Reformulators::KKT::add_kkt_reformulation(idol::Model &t_destination) {

    add_primal_variables(t_destination);
    add_dual_variables(t_destination);

    add_primal_constraints(t_destination);
    add_dual_constraints(t_destination);

    for (const auto& ctr : m_primal.ctrs())  {


        if (!m_primal_constraint_indicator(ctr)) {
            continue;
        }

        const auto type = m_primal.get_ctr_type(ctr);

        if (type == Equal) {
            continue;
        }

        const auto index = m_primal.get_ctr_index(ctr);
        const auto& dual_var = *m_dual_variables_for_constraints[index];
        const auto& row = m_primal.get_ctr_row(ctr);
        const auto rhs = m_primal.get_ctr_rhs(ctr);

        t_destination.add_qctr((row - rhs) * dual_var, Equal);
    }

    for (const auto& var : m_primal.vars()) {
        if (!m_primal_variable_indicator(var)) {
            continue;
        }
        const auto index = m_primal.get_var_index(var);
        const double lb = m_primal.get_var_lb(var);
        const double ub = m_primal.get_var_ub(var);

        if (!is_neg_inf(lb)) {
            const auto& dual_var = *m_dual_variables_for_lower_bounds[index];
            t_destination.add_qctr((var - lb) * dual_var, Equal);
        }

        if (!is_pos_inf(ub)) {
            const auto& dual_var = *m_dual_variables_for_upper_bounds[index];
            t_destination.add_qctr((ub - var) * dual_var, Equal);
        }

    }

}

void
idol::Reformulators::KKT::add_kkt_reformulation(idol::Model &t_destination, const idol::Annotation<double> &t_big_M) {

    add_primal_variables(t_destination);
    add_dual_variables(t_destination);

    add_primal_constraints(t_destination);
    add_dual_constraints(t_destination);

    for (const auto& ctr : m_primal.ctrs())  {

        if (!m_primal_constraint_indicator(ctr)) {
            continue;
        }

        const auto type = m_primal.get_ctr_type(ctr);

        if (type == Equal) {
            continue;
        }

        const auto index = m_primal.get_ctr_index(ctr);
        const auto& dual_var = *m_dual_variables_for_constraints[index];
        const auto& row = m_primal.get_ctr_row(ctr);
        const auto rhs = m_primal.get_ctr_rhs(ctr);

        const auto z = t_destination.add_var(0, 1, Binary, 0, "complementarity_" + ctr.name());

        t_destination.add_ctr(dual_var <= ctr.get(t_big_M) * z);

        if (type == LessOrEqual) {
            t_destination.add_ctr(row - rhs >= -ctr.get(t_big_M) * (1 - z));
        } else {
            t_destination.add_ctr(row - rhs <= ctr.get(t_big_M) * (1 - z));
        }

    }

    for (const auto& var : m_primal.vars()) {
        if (!m_primal_variable_indicator(var)) {
            continue;
        }
        const auto index = m_primal.get_var_index(var);
        const double lb = m_primal.get_var_lb(var);
        const double ub = m_primal.get_var_ub(var);

        if (!is_neg_inf(lb)) {
            const auto& dual_var = *m_dual_variables_for_lower_bounds[index];
            const auto z = t_destination.add_var(0, 1, Binary, 0, "complementarity_lb_" + var.name());
            t_destination.add_ctr(dual_var <= var.get(t_big_M) * z);
            t_destination.add_ctr(var - lb >= -var.get(t_big_M) * (1 - z));
        }

        if (!is_pos_inf(ub)) {
            const auto& dual_var = *m_dual_variables_for_upper_bounds[index];
            const auto z = t_destination.add_var(0, 1, Binary, 0, "complementarity_ub_" + var.name());
            t_destination.add_ctr(dual_var <= var.get(t_big_M) * z);
            t_destination.add_ctr(ub - var >= -var.get(t_big_M) * (1 - z));
        }

    }

}
