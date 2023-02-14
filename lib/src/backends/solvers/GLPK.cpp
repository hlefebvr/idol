//
// Created by henri on 14/02/23.
//
#include "backends/solvers/GLPK.h"
#include <glpk.h>

GLPK::GLPK(const AbstractModel &t_model) : LazyBackend(t_model), m_model(glp_create_prob()) {

}

void GLPK::hook_initialize() {

    const auto& objective = parent().get(Attr::Obj::Expr);

    if (!objective.quadratic().empty()) {
        throw Exception("GLPK is not available as an SOCP solver.");
    }

    hook_update_objective_sense();
    set_objective_as_updated();
    set_rhs_as_updated();

}

void GLPK::hook_write(const std::string &t_name) {
    glp_write_lp(m_model, nullptr, t_name.c_str());
}

int GLPK::hook_add(const Var &t_var, bool t_add_column) {

    int index;
    if (m_deleted_variables.empty()) {
        index = (int) glp_get_num_cols(m_model) + 1;
        glp_add_cols(m_model, 1);
    } else {
        index = m_deleted_variables.top();
        m_deleted_variables.pop();
    }

    const double lb = parent().get(Attr::Var::Lb, t_var);
    const double ub = parent().get(Attr::Var::Ub, t_var);
    const bool has_lb = !is_neg_inf(lb);
    const bool has_ub = !is_pos_inf(ub);

    if (has_lb && has_ub) {
        glp_set_col_bnds(m_model, index, GLP_DB, lb, ub);
    } else if (has_lb) {
        glp_set_col_bnds(m_model, index, GLP_LO, lb, 0.);
    } else if (has_ub) {
        glp_set_col_bnds(m_model, index, GLP_UP, 0., ub);
    } else {
        glp_set_col_bnds(m_model, index, GLP_FR, 0., 0.);
    }

    const auto& column = parent().get(Attr::Var::Column, t_var);
    const auto type = parent().get(Attr::Var::Type, t_var);

    glp_set_obj_coef(m_model, index, as_numeric(column.obj()));

    switch (type) {
        case Continuous: glp_set_col_kind(m_model, index, GLP_CV); break;
        case Binary: glp_set_col_kind(m_model, index, GLP_BV); break;
        case Integer: glp_set_col_kind(m_model, index, GLP_IV); break;
        default: throw std::runtime_error("Unknown variable type.");
    }

    glp_set_col_name(m_model, index, t_var.name().c_str());

    if (t_add_column) {

        const auto n = (int) column.linear().size();
        auto* coefficients = new double[n+1];
        auto* indices = new int[n+1];

        int i = 1;
        for (const auto& [ctr, coeff] : column.linear()) {
            indices[i] = lazy(ctr).impl();
            coefficients[i] = as_numeric(coeff);
            ++i;
        }

        glp_set_mat_col(m_model, index, n, indices, coefficients);

        delete[] indices;
        delete[] coefficients;

    }

    return index;
}

int GLPK::hook_add(const Ctr &t_ctr) {

    int index;
    if (m_deleted_constraints.empty()) {
        index = (int) glp_get_num_rows(m_model) + 1;
        glp_add_rows(m_model, 1);
    } else {
        index = m_deleted_constraints.top();
        m_deleted_constraints.pop();
    }

    const auto& row = parent().get(Attr::Ctr::Row, t_ctr);
    const double rhs = as_numeric(row.rhs());
    const auto type = parent().get(Attr::Ctr::Type, t_ctr);

    switch (type) {
        case LessOrEqual: glp_set_row_bnds(m_model, index, GLP_UP, 0., rhs); break;
        case GreaterOrEqual: glp_set_row_bnds(m_model, index, GLP_LO, rhs, 0.); break;
        case Equal: glp_set_row_bnds(m_model, index, GLP_FX, rhs, 0.); break;
        default: throw std::runtime_error("Unknown constraint type.");
    }

    glp_set_row_name(m_model, index, t_ctr.name().c_str());

    const auto n = (int) row.linear().size();
    auto* coefficients = new double[n+1];
    auto* indices = new int[n+1];

    int i = 1;
    for (const auto& [var, coeff] : row.linear()) {
        indices[i] = lazy(var).impl();
        coefficients[i] = as_numeric(coeff);
        ++i;
    }

    glp_set_mat_row(m_model, index, n, indices, coefficients);

    delete[] indices;
    delete[] coefficients;

    return index;
}

void GLPK::hook_update_objective_sense() {
    glp_set_obj_dir(m_model, parent().get(Attr::Obj::Sense) == Minimize ? GLP_MIN : GLP_MAX);
}

void GLPK::hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) {
    throw Exception("Not implemented.");
}

void GLPK::hook_update() {

}

void GLPK::hook_update(const Var &t_var) {
    throw Exception("Not implemented.");
}

void GLPK::hook_update(const Ctr &t_ctr) {
    throw Exception("Not implemented.");
}

void GLPK::hook_update_objective() {
    throw Exception("Not implemented.");
}

void GLPK::hook_update_rhs() {
    throw Exception("Not implemented.");
}

void GLPK::hook_remove(const Var &t_var) {
    throw Exception("Not implemented.");
}

void GLPK::hook_remove(const Ctr &t_ctr) {
    throw Exception("Not implemented.");
}

void GLPK::hook_optimize() {

    m_solved_as_mip = false;

    glp_smcp parameters;
    glp_init_smcp(&parameters);
    parameters.msg_lev = GLP_MSG_ERR;

    if (m_rebuild_basis) {
        glp_std_basis(m_model);
        m_rebuild_basis = false;
    }

    int result = glp_simplex(m_model, &parameters);

    if (result == GLP_ESING) {
        glp_std_basis(m_model);
        glp_simplex(m_model, &parameters);
    }

    save_simplex_solution_status();

    if (get(Param::Algorithm::InfeasibleOrUnboundedInfo)) {

        if (m_solution_status == Infeasible) {
            compute_farkas_certificate();
            return;
        } else if (m_solution_status == Unbounded) {
            compute_unbounded_ray();
            return;
        }

    }

    if (glp_get_num_int(m_model) > 0 && m_solution_status == Optimal) {

        glp_iocp parameters_integer;
        glp_init_iocp(&parameters_integer);
        parameters_integer.msg_lev = GLP_MSG_ERR;
        glp_intopt(m_model, &parameters_integer);
        m_solved_as_mip = true;

        save_milp_solution_status();

    }

}

void GLPK::save_simplex_solution_status() {

    int status = glp_get_status(m_model);

    if (status == GLP_UNDEF) {
        m_solution_status = Unknown;
        m_solution_reason = NotSpecified;
        return;
    }

    if (status == GLP_OPT) {
        m_solution_status = Optimal;
        m_solution_reason = Proved;
        return;
    }

    if (status == GLP_FEAS) {
        m_solution_status = Feasible;
        m_solution_reason = Proved;
        return;
    }

    if (status == GLP_INFEAS || status == GLP_NOFEAS) {
        m_solution_status = Infeasible;
        m_solution_reason = Proved;
        return;
    }

    if (status == GLP_UNBND) {
        m_solution_status = Unbounded;
        m_solution_reason = Proved;
        return;
    }

    throw Exception("GLPK: Unhandled solution status: " + std::to_string(status));

}

void GLPK::compute_farkas_certificate() {
    throw Exception("Not implemented.");
}

void GLPK::compute_unbounded_ray() {
    throw Exception("Not implemented.");
}

void GLPK::save_milp_solution_status() {

    int status = glp_mip_status(m_model);

    if (status == GLP_UNDEF) {
        m_solution_status = Unknown;
        m_solution_reason = NotSpecified;
        return;
    }

    if (status == GLP_OPT) {
        m_solution_status = Optimal;
        m_solution_reason = Proved;
        return;
    }

    if (status == GLP_FEAS) {
        m_solution_status = Feasible;
        m_solution_reason = Proved;
        return;
    }

    if (status == GLP_NOFEAS) {
        m_solution_status = Infeasible;
        m_solution_reason = Proved;
        return;
    }

    throw Exception("GLPK: Unhandled solution status: " + std::to_string(status));

}

void GLPK::set(const Parameter<bool> &t_param, bool t_value) {

    if (t_param.is_in_section(Param::Sections::Algorithm)) {
        m_bool_params.set(t_param, t_value);
        return;
    }

    Base::set(t_param, t_value);
}

bool GLPK::get(const Parameter<bool> &t_param) const {

    if (t_param.is_in_section(Param::Sections::Algorithm)) {
        return m_bool_params.get(t_param);
    }

    return LazyBackend::get(t_param);
}

int GLPK::get(const Req<int, void> &t_attr) const {

    if (t_attr == Attr::Solution::Status) {
        return m_solution_status;
    }

    if (t_attr == Attr::Solution::Reason) {
        return m_solution_reason;
    }

    return Base::get(t_attr);
}

double GLPK::get(const Req<double, void> &t_attr) const {

    if (t_attr == Attr::Solution::ObjVal) {
        if (m_solution_status == Unbounded) { return -Inf; }
        if (m_solution_status == Infeasible) { return +Inf; }
        const double constant_term = as_numeric(parent().get(Attr::Obj::Const));
        return constant_term + (m_solved_as_mip ? glp_mip_obj_val(m_model) : glp_get_obj_val(m_model));
    }

    return Base::get(t_attr);
}

double GLPK::get(const Req<double, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Solution::Primal) {
        const int impl = lazy(t_var).impl();
        return m_solved_as_mip ? glp_mip_col_val(m_model, impl) : glp_get_col_prim(m_model, impl);
    }

    return Base::get(t_attr, t_var);
}

double GLPK::get(const Req<double, Ctr> &t_attr, const Ctr &t_ctr) const {

    if (t_attr == Attr::Solution::Dual) {
        const auto &impl = lazy(t_ctr).impl();
        return glp_get_row_dual(m_model, impl);
    }

    return Base::get(t_attr, t_ctr);
}

