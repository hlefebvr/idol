//
// Created by henri on 14/02/23.
//
#include "optimizers/solvers/Optimizers_GLPK.h"
#include "optimizers/Algorithm.h"

#ifdef IDOL_USE_GLPK

Optimizers::GLPK::GLPK(const Model &t_model, bool t_continuous_relaxation)
    : OptimizerWithLazyUpdates(t_model),
      m_model(glp_create_prob()),
      m_continuous_relaxation(t_continuous_relaxation) {

    glp_init_smcp(&m_simplex_parameters);
    m_simplex_parameters.msg_lev = GLP_MSG_ERR;
    m_simplex_parameters.presolve = GLP_OFF;


    glp_init_iocp(&m_mip_parameters);
    m_mip_parameters.msg_lev = GLP_MSG_ERR;
    m_mip_parameters.presolve = GLP_OFF;

}

void Optimizers::GLPK::hook_build() {

    const auto& objective = parent().get_obj_expr();

    if (!objective.quadratic().empty()) {
        throw Exception("GLPK is not available as an SOCP solver.");
    }

    hook_update_objective_sense();
    set_objective_as_updated();
    set_rhs_as_updated();

}

void Optimizers::GLPK::hook_write(const std::string &t_name) {
    glp_write_lp(m_model, nullptr, t_name.c_str());
}

void Optimizers::GLPK::set_var_attr(int t_index, int t_type, double t_lb, double t_ub, double t_obj) {

    const bool has_lb = !is_neg_inf(t_lb);
    const bool has_ub = !is_pos_inf(t_ub);

    // Set obj
    glp_set_obj_coef(m_model, t_index, t_obj);

    // Set type
    if (m_continuous_relaxation || t_type == Continuous) {
        glp_set_col_kind(m_model, t_index, GLP_CV);
    } else if (t_type == Binary) {
        glp_set_col_kind(m_model, t_index, GLP_BV);
    } else if (t_type == Integer) {
        glp_set_col_kind(m_model, t_index, GLP_IV);
    } else {
        throw std::runtime_error("Unknown variable type.");
    }

    // Set bounds
    if (has_lb && has_ub) {
        if (equals(t_lb, t_ub, ToleranceForIntegrality)) {
            glp_set_col_bnds(m_model, t_index, GLP_FX, t_lb, t_ub);
        } else {
            glp_set_col_bnds(m_model, t_index, GLP_DB, t_lb, t_ub);
        }
    } else if (has_lb) {
        glp_set_col_bnds(m_model, t_index, GLP_LO, t_lb, 0.);
    } else if (has_ub) {
        glp_set_col_bnds(m_model, t_index, GLP_UP, 0., t_ub);
    } else {
        glp_set_col_bnds(m_model, t_index, GLP_FR, 0., 0.);
    }

}

int Optimizers::GLPK::hook_add(const Var &t_var, bool t_add_column) {

    int index;
    if (m_deleted_variables.empty()) {
        index = (int) glp_get_num_cols(m_model) + 1;
        glp_add_cols(m_model, 1);
    } else {
        index = m_deleted_variables.top();
        m_deleted_variables.pop();
    }

    const double lb = parent().get_var_lb(t_var);
    const double ub = parent().get_var_ub(t_var);
    const auto& column = parent().get_var_column(t_var);
    const auto type = parent().get_var_type(t_var);

    set_var_attr(index, type, lb, ub, as_numeric(column.obj()));

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

void Optimizers::GLPK::set_ctr_attr(int t_index, int t_type, double t_rhs) {

    switch (t_type) {
        case LessOrEqual: glp_set_row_bnds(m_model, t_index, GLP_UP, 0., t_rhs); break;
        case GreaterOrEqual: glp_set_row_bnds(m_model, t_index, GLP_LO, t_rhs, 0.); break;
        case Equal: glp_set_row_bnds(m_model, t_index, GLP_FX, t_rhs, 0.); break;
        default: throw std::runtime_error("Unknown constraint type.");
    }

}

int Optimizers::GLPK::hook_add(const Ctr &t_ctr) {

    int index;
    if (m_deleted_constraints.empty()) {
        index = (int) glp_get_num_rows(m_model) + 1;
        glp_add_rows(m_model, 1);
    } else {
        index = m_deleted_constraints.top();
        m_deleted_constraints.pop();
    }

    const auto& row = parent().get_ctr_row(t_ctr);
    const double rhs = as_numeric(row.rhs());
    const auto type = parent().get_ctr_type(t_ctr);

    set_ctr_attr(index, type, rhs);

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

void Optimizers::GLPK::hook_update_objective_sense() {
    glp_set_obj_dir(m_model, parent().get_obj_sense() == Minimize ? GLP_MIN : GLP_MAX);
}

void Optimizers::GLPK::hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) {
    throw Exception("Not implemented.");
}

void Optimizers::GLPK::hook_update() {

}

void Optimizers::GLPK::hook_update(const Var &t_var) {

    const auto& model = parent();
    auto& impl = lazy(t_var).impl();
    const double lb = model.get_var_lb(t_var);
    const double ub = model.get_var_ub(t_var);
    const int type = model.get_var_type(t_var);
    const Constant& obj = model.get_var_column(t_var).obj();

    set_var_attr(impl, type, lb, ub, as_numeric(obj));

}

void Optimizers::GLPK::hook_update(const Ctr &t_ctr) {

    const auto& model = parent();
    auto& impl = lazy(t_ctr).impl();
    const auto& rhs = model.get_ctr_row(t_ctr).rhs();
    const auto type = model.get_ctr_type(t_ctr);

    set_ctr_attr(impl, type, as_numeric(rhs));

}

void Optimizers::GLPK::hook_update_objective() {

    const auto& model = parent();

    for (const auto& var : model.vars()) {
        const auto& obj = model.get_var_column(var).obj();
        glp_set_obj_coef(m_model, lazy(var).impl(), as_numeric(obj));
    }

}

void Optimizers::GLPK::hook_update_rhs() {
    throw Exception("Not implemented.");
}

void Optimizers::GLPK::hook_remove(const Var &t_var) {

    const int index = lazy(t_var).impl();

    if (glp_get_col_stat(m_model, index) != GLP_NF) {
        m_rebuild_basis = true;
    }

    glp_set_obj_coef(m_model, index, 0.);
    glp_set_mat_col(m_model, index, 0, NULL, NULL);
    glp_set_col_name(m_model, index, ("_removed_" + t_var.name()).c_str());
    m_deleted_variables.push(index);

}

void Optimizers::GLPK::hook_remove(const Ctr &t_ctr) {

    const int index = lazy(t_ctr).impl();

    if (glp_get_row_stat(m_model, index) != GLP_NF) {
        m_rebuild_basis = true;
    }

    glp_set_row_bnds(m_model, index, GLP_FX, 0., 0.);
    glp_set_mat_row(m_model, index, 0, NULL, NULL);
    glp_set_col_name(m_model, index, ("_removed_" + t_ctr.name()).c_str());
    m_deleted_constraints.push(index);

}

void Optimizers::GLPK::hook_optimize() {

    m_solved_as_mip = false;

    if (m_solution_status == Unbounded) {
        m_rebuild_basis = true;
    }

    if (m_rebuild_basis) {
        glp_std_basis(m_model);
        m_rebuild_basis = false;
    }

    int result = glp_simplex(m_model, &m_simplex_parameters);

    if (result == GLP_ESING) {
        glp_std_basis(m_model);
        glp_simplex(m_model, &m_simplex_parameters);
    }

    save_simplex_solution_status();

    if (infeasible_or_unbounded_info()) {

        if (m_solution_status == Infeasible) {
            compute_farkas_certificate();
            return;
        }

        if (m_solution_status == Unbounded) {
            compute_unbounded_ray();
            return;
        }

    }

    if (glp_get_num_int(m_model) > 0 && m_solution_status == Optimal) {

        glp_intopt(m_model, &m_mip_parameters);
        m_solved_as_mip = true;

        save_milp_solution_status();

    }

}

void Optimizers::GLPK::save_simplex_solution_status() {

    int status = glp_get_status(m_model);

    if (status == GLP_UNDEF) {
        m_solution_status = Fail;
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

void Optimizers::GLPK::compute_farkas_certificate() {
    
    const auto& model = parent();

    std::list<std::pair<int, int>> basis_rows;
    std::list<std::pair<int, int>> basis_cols;
    for (const auto& ctr : model.ctrs()) {
        int index = lazy(ctr).impl();
        int status = glp_get_row_stat(m_model, index);
        basis_rows.emplace_back(index, status);
    }
    for (const auto& var : model.vars()) {
        int index = lazy(var).impl();
        int status = glp_get_col_stat(m_model, index);
        basis_cols.emplace_back(index, status);
    }

    std::vector<int> artificial_variables;
    artificial_variables.reserve(2 * model.ctrs().size() + 1);
    artificial_variables.emplace_back(0);

    auto* minus_one = new double[2]; minus_one[1] = -1.;
    auto* plus_one = new double[2]; plus_one[1] = 1.;

    // Add artificial variables
    for (const auto& ctr : model.ctrs()) {

        auto* index = new int[2];
        index[1] = lazy(ctr).impl();
        const auto type = model.get_ctr_type(ctr);

        if (type == LessOrEqual) {
            int art_var_index = glp_add_cols(m_model, 1);
            glp_set_mat_col(m_model, art_var_index, 1, index, minus_one);
            glp_set_obj_coef(m_model, art_var_index, 1);
            glp_set_col_bnds(m_model, art_var_index, GLP_LO, 0., 0.);
            artificial_variables.emplace_back(art_var_index);
        } else if (type == GreaterOrEqual) {
            int art_var_index = glp_add_cols(m_model, 1);
            glp_set_mat_col(m_model, art_var_index, 1, index, plus_one);
            glp_set_obj_coef(m_model, art_var_index, 1);
            glp_set_col_bnds(m_model, art_var_index, GLP_LO, 0., 0.);
            artificial_variables.emplace_back(art_var_index);
        } else {
            int art_var_index = glp_add_cols(m_model, 2);
            glp_set_mat_col(m_model, art_var_index, 1, index, minus_one);
            glp_set_obj_coef(m_model, art_var_index, 1);
            glp_set_col_bnds(m_model, art_var_index, GLP_LO, 0., 0.);
            glp_set_mat_col(m_model, art_var_index + 1, 1, index, plus_one);
            glp_set_obj_coef(m_model, art_var_index + 1, 1);
            glp_set_col_bnds(m_model, art_var_index + 1, GLP_LO, 0., 0.);
            artificial_variables.emplace_back(art_var_index);
            artificial_variables.emplace_back(art_var_index + 1);
        }

        delete[] index;

    }

    delete[] minus_one;
    delete[] plus_one;

    // Set original variables' objective coefficient to zero
    for (const auto& [var, constant] : model.get_obj_expr().linear()) {
        glp_set_obj_coef(m_model, lazy(var).impl(), 0.);
    }

    // Solve feasible model
    glp_simplex(m_model, &m_simplex_parameters);

    // Save dual values as Farkas certificate
    m_farkas_certificate = Solution::Dual();
    double objective_value = as_numeric(model.get_obj_expr().constant());
    for (const auto& ctr : model.ctrs()) {
        const double dual = glp_get_row_dual(m_model, lazy(ctr).impl());
        m_farkas_certificate->set(ctr, dual);
        objective_value += dual * as_numeric(model.get_ctr_row(ctr).rhs());
    }
    m_farkas_certificate->set_objective_value(objective_value);

    // Remove added variables
    glp_del_cols(m_model, (int) artificial_variables.size() - 1, artificial_variables.data());

    // Restore objective function
    for (const auto& [var, constant] : model.get_obj_expr().linear()) {
        glp_set_obj_coef(m_model, lazy(var).impl(), as_numeric(constant));
    }

    // Restore basis
    for (const auto& [index, status] : basis_rows) {
        glp_set_row_stat(m_model, index, status);
    }
    for (const auto& [index, status] : basis_cols) {
        glp_set_col_stat(m_model, index, status);
    }

}

void Optimizers::GLPK::compute_unbounded_ray() {

    const auto& model = parent();

    // Set model RHS to zero
    for (const auto& var : model.vars()) {

        const int index = lazy(var).impl();
        const double lb = model.get_var_lb(var);
        const double ub = model.get_var_ub(var);
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

    }

    for (const auto& ctr : model.ctrs()) {
        const int index = lazy(ctr).impl();
        const auto type = model.get_ctr_type(ctr);
        set_ctr_attr(index, type, 0.);
    }

    // Create absolute value variables
    const auto n_variables = glp_get_num_cols(m_model);
    const auto n_constraints = glp_get_num_rows(m_model);
    glp_add_cols(m_model, n_variables);
    glp_add_rows(m_model, 2 * n_variables + 1);

    for (int j = 1 ; j <= n_variables ; ++j) {

        std::vector<int> indices { 0, j, n_variables + j };

        std::vector<double> coefficients { 0, 1, -1 };
        glp_set_row_bnds(m_model, n_constraints + j, GLP_UP, 0., 0.);
        glp_set_mat_row(m_model, n_constraints + j, 2, indices.data(), coefficients.data());

        coefficients = { 0, -1, -1 };
        glp_set_row_bnds(m_model, n_constraints + n_variables + j, GLP_UP, 0., 0.);
        glp_set_mat_row(m_model, n_constraints + n_variables + j, 2, indices.data(), coefficients.data());

        set_var_attr(n_variables + j, Continuous, 0., 1., 0.);

    }

    // Create l1-norm
    std::vector<int> indices;
    indices.reserve(n_variables + 1);
    indices.emplace_back(0);

    std::vector<double> coefficients;
    coefficients.reserve(n_variables + 1);
    coefficients.emplace_back(0);

    for (int j = 1 ; j <= n_variables ; ++j) {
        indices.emplace_back(n_variables + j);
        coefficients.emplace_back(1);
    }
    glp_set_row_bnds(m_model, n_constraints + 2 * n_variables + 1, GLP_UP, 0., 1.);
    glp_set_mat_row(m_model, n_constraints + 2 * n_variables + 1, n_variables, indices.data(), coefficients.data());

    glp_std_basis(m_model);
    glp_simplex(m_model, &m_simplex_parameters);

    // Save ray
    m_unbounded_ray = Solution::Primal();
    const double objective_value = as_numeric(model.get_obj_expr().constant()) + glp_get_obj_val(m_model);
    m_unbounded_ray->set_objective_value(objective_value);
    for (const auto& var : model.vars()) {
        m_unbounded_ray->set(var, glp_get_col_prim(m_model, lazy(var).impl()));
    }

    // Restore model

    // Remove created variables
    glp_del_cols(m_model, n_variables, indices.data());

    // Remove created rows
    std::vector<int> created_rows;
    created_rows.reserve(2 * n_variables + 2);
    created_rows.emplace_back(0);
    for (int j = 1 ; j <= n_variables ; ++j) {
        created_rows.emplace_back(n_constraints + j);
        created_rows.emplace_back(n_constraints + n_variables + j);
    }
    created_rows.emplace_back(n_constraints + 2 * n_variables + 1);
    glp_del_rows(m_model, (int) created_rows.size() - 1, created_rows.data());

    // Restore variables bounds
    for (const auto& var : model.vars()) {
        const int index = lazy(var).impl();
        const int type = model.get_var_type(var);
        const double lb = model.get_var_lb(var);
        const double ub = model.get_var_ub(var);
        const auto& column = model.get_var_column(var);
        const double obj = as_numeric(column.obj());
        set_var_attr(index, type, lb, ub, obj);
    }

    // Restore RHS
    for (const auto& ctr : model.ctrs()) {
        const int index = lazy(ctr).impl();
        const int type = model.get_ctr_type(ctr);
        const auto& row = model.get_ctr_row(ctr);
        const double rhs = as_numeric(row.rhs());
        set_ctr_attr(index, type, rhs);
    }

}

void Optimizers::GLPK::save_milp_solution_status() {

    int status = glp_mip_status(m_model);

    if (status == GLP_UNDEF) {
        m_solution_status = Fail;
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

void Optimizers::GLPK::set_time_limit(double t_time_limit) {

    const int value = (int) std::min<double>(std::numeric_limits<int>::max(), std::ceil(t_time_limit) * 1000);
    m_simplex_parameters.tm_lim = value;
    m_mip_parameters.tm_lim = value;

    Optimizer::set_time_limit(t_time_limit);
}

void Optimizers::GLPK::set_best_obj_stop(double t_best_obj_stop) {
    m_simplex_parameters.obj_ul = t_best_obj_stop;
    Optimizer::set_best_obj_stop(t_best_obj_stop);
}

void Optimizers::GLPK::set_best_bound_stop(double t_best_bound_stop) {
    m_simplex_parameters.obj_ll = t_best_bound_stop;
    Optimizer::set_best_bound_stop(t_best_bound_stop);
}

void Optimizers::GLPK::set_presolve(bool t_value) {
    m_simplex_parameters.presolve = t_value ? GLP_MSG_ERR : GLP_MSG_OFF;
    m_mip_parameters.presolve = t_value ? GLP_MSG_ERR : GLP_MSG_OFF;
    Optimizer::set_presolve(t_value);
}

SolutionStatus Optimizers::GLPK::get_status() const {
    return m_solution_status;
}

SolutionReason Optimizers::GLPK::get_reason() const {
    return m_solution_reason;
}

double Optimizers::GLPK::get_best_obj() const {
    if (m_solution_status == Unbounded) { return -Inf; }
    if (m_solution_status == Infeasible) { return +Inf; }
    const double constant_term = as_numeric(parent().get_obj_expr().constant());
    return constant_term + (m_solved_as_mip ? glp_mip_obj_val(m_model) : glp_get_obj_val(m_model));
}

double Optimizers::GLPK::get_best_bound() const {
    return get_best_obj();
}

double Optimizers::GLPK::get_var_primal(const Var &t_var) const {
    const int impl = lazy(t_var).impl();
    return m_solved_as_mip ? glp_mip_col_val(m_model, impl) : glp_get_col_prim(m_model, impl);
}

double Optimizers::GLPK::get_var_ray(const Var &t_var) const {

    if (!m_unbounded_ray.has_value()) {
        throw Exception("Ray not available.");
    }

    return m_unbounded_ray->get(t_var);
}

double Optimizers::GLPK::get_ctr_dual(const Ctr &t_ctr) const {
    const auto &impl = lazy(t_ctr).impl();
    return glp_get_row_dual(m_model, impl);
}

double Optimizers::GLPK::get_ctr_farkas(const Ctr &t_ctr) const {
    if (!m_farkas_certificate.has_value()) {
        throw Exception("Farkas certificate not available.");
    }

    return m_farkas_certificate->get(t_ctr);
}

double Optimizers::GLPK::get_relative_gap() const {
    return 0;
}

double Optimizers::GLPK::get_absolute_gap() const {
    return 0;
}

unsigned int Optimizers::GLPK::get_n_solutions() const {
    const auto status = get_status();
    return status == Optimal || status == Feasible;
}

unsigned int Optimizers::GLPK::get_solution_index() const {
    return 0;
}

void Optimizers::GLPK::set_solution_index(unsigned int t_index) {
    if (t_index != 0) {
        throw Exception("Solution index out of bounds");
    }
}

#endif
