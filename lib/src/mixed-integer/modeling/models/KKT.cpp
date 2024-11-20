//
// Created by henri on 30.08.24.
//

#include "idol/mixed-integer/modeling/models/KKT.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"
#include "idol/mixed-integer/modeling/constraints/TempQCtr.h"

idol::Reformulators::KKT::KKT(const idol::Model &t_src_model,
                              const idol::Bilevel::Description &t_lower_level_description)
    : m_src_model(t_src_model), m_description(t_lower_level_description) {

    create_dual_variables_for_constraints();
    create_dual_variables_for_bounds();
    create_dual_constraints();
    create_complementarity_constraints();
    create_dual_objective();

}

void idol::Reformulators::KKT::add_primal_variables(idol::Model &t_destination) const {

    for (const auto& var : m_src_model.vars()) {

        if (m_description.is_follower(var)) {
            t_destination.add(var, TempVar(-Inf, Inf, Continuous, 0., LinExpr<Ctr>()));
            continue;
        }

        const auto lb = m_src_model.get_var_lb(var);
        const auto ub = m_src_model.get_var_ub(var);
        const auto type = m_src_model.get_var_type(var);
        t_destination.add(var, TempVar(lb, ub, type, 0., LinExpr<Ctr>()));
    }

}

void idol::Reformulators::KKT::add_primal_constraints(Model &t_destination) const {

    for (const auto& ctr : m_src_model.ctrs()) {
        const auto& row = m_src_model.get_ctr_row(ctr);
        const auto type = m_src_model.get_ctr_type(ctr);
        const double rhs = m_src_model.get_ctr_rhs(ctr);
        t_destination.add(ctr, TempCtr(LinExpr<Var>(row), type, rhs));
    }

    for (const auto& var : m_src_model.vars()) {

        if (m_description.is_leader(var)) {
            continue;
        }

        const auto lb = m_src_model.get_var_lb(var);
        const auto ub = m_src_model.get_var_ub(var);

        if (!is_pos_inf(ub)) {
            t_destination.add_ctr(var <= ub, "ub_" + var.name());
        }

        if (!is_neg_inf(lb)) {
            t_destination.add_ctr(var >= lb, "lb_" + var.name());
        }

    }

}

void idol::Reformulators::KKT::create_dual_variables_for_constraints() {

    auto& env = m_src_model.env();
    m_dual_variables_for_constraints.resize(m_src_model.ctrs().size());

    for (const auto& ctr : m_src_model.ctrs()) {

        if (m_description.is_leader(ctr)) {
            continue;
        }

        const auto type = m_src_model.get_ctr_type(ctr);
        const auto index = m_src_model.get_ctr_index(ctr);

        double lb, ub;
        switch (type) {
            case LessOrEqual: lb = -Inf; ub = 0; break;
            case Equal: lb = -Inf; ub = Inf; break;
            case GreaterOrEqual: lb = 0; ub = Inf; break;
        }

        m_dual_variables_for_constraints[index] = Var(env,  lb, ub, Continuous, 0., "dual_" + ctr.name());

    }

}

void idol::Reformulators::KKT::create_dual_variables_for_bounds() {

    auto& env = m_src_model.env();
    m_dual_variables_for_lower_bounds.resize(m_src_model.vars().size());
    m_dual_variables_for_upper_bounds.resize(m_src_model.vars().size());

    for (const auto& var : m_src_model.vars()) {

        if (m_description.is_leader(var)) {
            continue;
        }

        const auto lb = m_src_model.get_var_lb(var);
        const auto ub = m_src_model.get_var_ub(var);
        const auto index = m_src_model.get_var_index(var);

        if (!is_pos_inf(ub)) {
            m_dual_variables_for_upper_bounds[index] = Var(env, -Inf, 0, Continuous, 0., "dual_ub_" + var.name());
        }

        if (!is_neg_inf(lb)) {
            m_dual_variables_for_lower_bounds[index] = Var(env, 0, Inf, Continuous, 0., "dual_lb_" + var.name());
        }

    }

}

void idol::Reformulators::KKT::add_dual_variables(idol::Model &t_destination) const {

    for (const auto& opt_var : m_dual_variables_for_constraints) {
        if (opt_var.has_value()) {
            t_destination.add(opt_var.value());
        }
    }

    for (const auto& opt_var : m_dual_variables_for_lower_bounds) {
        if (opt_var.has_value()) {
            t_destination.add(opt_var.value());
        }
    }

    for (const auto& opt_var : m_dual_variables_for_upper_bounds) {
        if (opt_var.has_value()) {
            t_destination.add(opt_var.value());
        }
    }

}

void idol::Reformulators::KKT::create_dual_constraints() {

    m_dual_constraints.resize(m_src_model.vars().size());

    for (const auto& var : m_src_model.vars()) {

        if (m_description.is_leader(var)) {
            continue;
        }

        create_dual_constraint(var);

    }

}

void idol::Reformulators::KKT::create_dual_constraint(const idol::Var &t_var) {

    auto& env = m_src_model.env();
    const auto index = m_src_model.get_var_index(t_var);
    const auto& col = m_src_model.get_var_column(t_var);

    AffExpr expr;

    for (const auto& [ctr, constant] : col) {

        if (m_description.is_leader(ctr)) {
            continue;
        }

        const auto index_ctr = m_src_model.get_ctr_index(ctr);
        const auto& dual_var = *m_dual_variables_for_constraints[index_ctr];
        expr += constant * dual_var;

    }

    if (const auto dual_var = m_dual_variables_for_lower_bounds[index]; dual_var.has_value()) {
        expr += dual_var.value();
    }

    if (const auto dual_var = m_dual_variables_for_upper_bounds[index]; dual_var.has_value()) {
        expr += dual_var.value();
    }

    AffExpr obj = m_description.follower_obj().linear().get(t_var);

    m_dual_constraints[index] = Ctr(env, expr == obj, "dual_" + t_var.name());

}

void idol::Reformulators::KKT::create_complementarity_constraints() {

    m_complementarity_constraints.resize(m_src_model.ctrs().size());
    auto& env = m_src_model.env();

    for (const auto& ctr : m_src_model.ctrs()) {

        if (m_description.is_leader(ctr)) {
            continue;
        }

        const auto type = m_src_model.get_ctr_type(ctr);

        if (type == Equal) {
            continue;
        }

        const auto index = m_src_model.get_ctr_index(ctr);
        const auto& dual_var = *m_dual_variables_for_constraints[index];
        const auto& row = m_src_model.get_ctr_row(ctr);
        const double rhs = m_src_model.get_ctr_rhs(ctr);

        QuadExpr expr;

        for (const auto& [var, constant] : row) {
            expr += constant * (var * dual_var);
        }

        expr -= rhs * dual_var;

        m_complementarity_constraints[index] = QCtr(env, TempQCtr(std::move(expr), Equal), "complementarity_" + ctr.name());

    }

}

void idol::Reformulators::KKT::add_dual_constraints(idol::Model &t_destination) const {

    for (const auto& opt_ctr : m_dual_constraints) {
        if (opt_ctr.has_value()) {
            t_destination.add(opt_ctr.value());
        }
    }

}

void idol::Reformulators::KKT::add_complementarity_constraints(idol::Model &t_destination) const {

    for (const auto& opt_ctr : m_complementarity_constraints) {
        if (opt_ctr.has_value()) {
            t_destination.add(opt_ctr.value());
        }
    }

}

void idol::Reformulators::KKT::add_leader_objective(idol::Model &t_destination) const {
    t_destination.set_obj_expr(m_src_model.get_obj_expr());
}

void idol::Reformulators::KKT::add_strong_duality_constraint(idol::Model &t_destination) const {

    t_destination.add_qctr(m_description.follower_obj() - m_dual_objective, LessOrEqual, "strong_duality");

}

void idol::Reformulators::KKT::create_dual_objective() {

    for (const auto& ctr : m_src_model.ctrs()) {

        if (m_description.is_leader(ctr)) {
            continue;
        }

        const auto index = m_src_model.get_ctr_index(ctr);
        const auto& dual_var = *m_dual_variables_for_constraints[index];
        const auto& row = m_src_model.get_ctr_row(ctr);
        const double rhs = m_src_model.get_ctr_rhs(ctr);

        for (const auto& [var, constant] : row) {
            if (m_description.is_follower(var)) {
                continue;
            }
            m_dual_objective -= constant * (var * dual_var);
        }

        m_dual_objective += rhs * dual_var;

    }

    for (const auto& var : m_src_model.vars()) {

        if (m_description.is_leader(var)) {
            continue;
        }

        const auto index = m_src_model.get_var_index(var);
        const auto lb = m_src_model.get_var_lb(var);
        const auto ub = m_src_model.get_var_ub(var);

        if (!is_neg_inf(lb)) {
            const auto& dual_var_lb = *m_dual_variables_for_lower_bounds[index];
            m_dual_objective += lb * dual_var_lb;
        }

        if (!is_pos_inf(ub)) {
            const auto& dual_var_ub = *m_dual_variables_for_upper_bounds[index];
            m_dual_objective += ub * dual_var_ub;
        }

    }

}

void idol::Reformulators::KKT::add_kkt_reformulation(idol::Model &t_destination) const {
    add_primal_variables(t_destination);
    add_primal_constraints(t_destination);
    add_dual_variables(t_destination);
    add_dual_constraints(t_destination);
    add_complementarity_constraints(t_destination);
    add_leader_objective(t_destination);
}

void idol::Reformulators::KKT::add_strong_duality_reformulation(idol::Model &t_destination) const {
    add_primal_variables(t_destination);
    add_primal_constraints(t_destination);
    add_dual_variables(t_destination);
    add_dual_constraints(t_destination);
    add_strong_duality_constraint(t_destination);
    add_leader_objective(t_destination);
}

void idol::Reformulators::KKT::add_dual(idol::Model &t_destination) const {
    add_dual_variables(t_destination);
    add_dual_constraints(t_destination);
    t_destination.set_obj_expr(-m_dual_objective);
}
