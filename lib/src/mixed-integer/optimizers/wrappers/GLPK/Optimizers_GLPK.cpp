//
// Created by henri on 14/02/23.
//
#include "idol/mixed-integer/optimizers/wrappers/GLPK/Optimizers_GLPK.h"
#include "idol/general/optimizers/Algorithm.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/general/utils/Finally.h"

#ifdef IDOL_USE_GLPK

idol::Optimizers::GLPK::GLPK(const Model &t_model, bool t_continuous_relaxation)
    : OptimizerWithLazyUpdates(t_model),
      m_model(glp_create_prob()),
      m_continuous_relaxation(t_continuous_relaxation) {

    glp_init_smcp(&m_simplex_parameters);
    m_simplex_parameters.msg_lev = GLP_MSG_OFF;
    //m_simplex_parameters.presolve = GLP_ON;

    glp_init_iocp(&m_mip_parameters);
    m_mip_parameters.msg_lev = GLP_MSG_OFF;
    //m_mip_parameters.presolve = GLP_ON;

}

void idol::Optimizers::GLPK::hook_build() {

    const auto& objective = parent().get_obj_expr();

    hook_update_objective_sense();
    set_objective_as_updated();
    set_rhs_as_updated();

}

void idol::Optimizers::GLPK::hook_write(const std::string &t_name) {
    glp_write_lp(m_model, nullptr, t_name.c_str());
}

void idol::Optimizers::GLPK::set_var_attr(int t_index, int t_type, double t_lb, double t_ub, double t_obj) {

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
        if (equals(t_lb, t_ub, Tolerance::Integer)) {
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

int idol::Optimizers::GLPK::hook_add(const Var &t_var, bool t_add_column) {

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
    const double obj = parent().get_var_obj(t_var);
    const auto type = parent().get_var_type(t_var);

    set_var_attr(index, type, lb, ub, obj);

    glp_set_col_name(m_model, index, t_var.name().c_str());

    if (t_add_column) {

        auto n = (int) column.size();
        auto* coefficients = new double[n+1];
        auto* indices = new int[n+1];

        int i = 1;
        for (const auto& [ctr, constant] : column) {
            if (!has_lazy(ctr)) { // if the constraint has no lazy, it will be created right after
                --n;
                continue;
            }
            indices[i] = lazy(ctr).impl();
            coefficients[i] = constant;
            ++i;
        }

        glp_set_mat_col(m_model, index, n, indices, coefficients);

        delete[] indices;
        delete[] coefficients;

    }

    return index;
}

void idol::Optimizers::GLPK::set_ctr_attr(int t_index, int t_type, double t_rhs) {

    switch (t_type) {
        case LessOrEqual: glp_set_row_bnds(m_model, t_index, GLP_UP, 0., t_rhs); break;
        case GreaterOrEqual: glp_set_row_bnds(m_model, t_index, GLP_LO, t_rhs, 0.); break;
        case Equal: glp_set_row_bnds(m_model, t_index, GLP_FX, t_rhs, 0.); break;
        default: throw std::runtime_error("Unknown constraint type.");
    }

}

int idol::Optimizers::GLPK::hook_add(const Ctr &t_ctr) {

    const auto& model = parent();

    int index;
    if (m_deleted_constraints.empty()) {
        index = (int) glp_get_num_rows(m_model) + 1;
        glp_add_rows(m_model, 1);
    } else {
        index = m_deleted_constraints.top();
        m_deleted_constraints.pop();
    }

    const auto& row = parent().get_ctr_row(t_ctr);
    const double rhs = parent().get_ctr_rhs(t_ctr);
    const auto type = parent().get_ctr_type(t_ctr);

    set_ctr_attr(index, type, rhs);

    glp_set_row_name(m_model, index, t_ctr.name().c_str());

    auto n = (int) row.size();
    auto* coefficients = new double[n+1];
    auto* indices = new int[n+1];

    int i = 1;
    for (const auto& [var, constant] : row) {
        if (!model.has(var)) {
            --n;
            continue;
        }
        indices[i] = lazy(var).impl();
        coefficients[i] = constant;
        ++i;
    }

    glp_set_mat_row(m_model, index, n, indices, coefficients);

    delete[] indices;
    delete[] coefficients;

    return index;
}

void idol::Optimizers::GLPK::hook_update_objective_sense() {
    glp_set_obj_dir(m_model, parent().get_obj_sense() == Minimize ? GLP_MIN : GLP_MAX);
}

void idol::Optimizers::GLPK::hook_update_matrix(const Ctr &t_ctr, const Var &t_var, double t_constant) {
    throw Exception("Not implemented.");
}

void idol::Optimizers::GLPK::hook_update() {

}

void idol::Optimizers::GLPK::hook_update(const Var &t_var) {

    const auto& model = parent();
    auto& impl = lazy(t_var).impl();
    const double lb = model.get_var_lb(t_var);
    const double ub = model.get_var_ub(t_var);
    const int type = model.get_var_type(t_var);
    const double obj = model.get_var_obj(t_var);

    set_var_attr(impl, type, lb, ub, obj);

}

void idol::Optimizers::GLPK::hook_update(const Ctr &t_ctr) {

    const auto& model = parent();
    auto& impl = lazy(t_ctr).impl();
    const auto& rhs = model.get_ctr_rhs(t_ctr);
    const auto type = model.get_ctr_type(t_ctr);

    set_ctr_attr(impl, type, rhs);

}

void idol::Optimizers::GLPK::hook_update_objective() {

    const auto& model = parent();

    for (const auto& var : model.vars()) {
        const double obj = model.get_var_obj(var);
        glp_set_obj_coef(m_model, lazy(var).impl(), obj);
    }

}

void idol::Optimizers::GLPK::hook_update_rhs() {
    throw Exception("Not implemented.");
}

void idol::Optimizers::GLPK::hook_remove(const Var &t_var) {

    const int index = lazy(t_var).impl();

    if (glp_get_col_stat(m_model, index) != GLP_NF) {
        m_rebuild_basis = true;
    }

    glp_set_obj_coef(m_model, index, 0.);
    glp_set_mat_col(m_model, index, 0, NULL, NULL);
    glp_set_col_name(m_model, index, ("_removed_" + t_var.name()).c_str());
    m_deleted_variables.push(index);

}

void idol::Optimizers::GLPK::hook_remove(const Ctr &t_ctr) {

    const int index = lazy(t_ctr).impl();

    if (glp_get_row_stat(m_model, index) != GLP_NF) {
        m_rebuild_basis = true;
    }

    glp_set_row_bnds(m_model, index, GLP_FX, 0., 0.);
    glp_set_mat_row(m_model, index, 0, NULL, NULL);
    glp_set_row_name(m_model, index, ("_removed_" + t_ctr.name()).c_str());
    m_deleted_constraints.push(index);

}

void idol::Optimizers::GLPK::hook_optimize() {

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

    if (get_param_infeasible_or_unbounded_info()) {

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

void idol::Optimizers::GLPK::save_simplex_solution_status() {

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

void idol::Optimizers::GLPK::compute_farkas_certificate() {
    
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
    for (const auto& var : model.vars()) {
        glp_set_obj_coef(m_model, lazy(var).impl(), 0.);
    }

    // Solve feasible model
    glp_simplex(m_model, &m_simplex_parameters);

    // Save dual values as Farkas certificate
    m_farkas_certificate = DualPoint();
    double objective_value = model.get_obj_expr().affine().constant();
    for (const auto& ctr : model.ctrs()) {
        const double dual = glp_get_row_dual(m_model, lazy(ctr).impl());
        m_farkas_certificate->set(ctr, dual);
        objective_value += dual * model.get_ctr_rhs(ctr);
    }
    m_farkas_certificate->set_objective_value(objective_value);

    // Remove added variables
    glp_del_cols(m_model, (int) artificial_variables.size() - 1, artificial_variables.data());

    // Restore objective function
    for (const auto& var : model.vars()) {
        glp_set_obj_coef(m_model, lazy(var).impl(), model.get_var_obj(var));
    }

    // Restore basis
    for (const auto& [index, status] : basis_rows) {
        glp_set_row_stat(m_model, index, status);
    }
    for (const auto& [index, status] : basis_cols) {
        glp_set_col_stat(m_model, index, status);
    }

}

void idol::Optimizers::GLPK::compute_unbounded_ray() {

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
    m_unbounded_ray = PrimalPoint();
    const double objective_value = model.get_obj_expr().affine().constant() + glp_get_obj_val(m_model);
    m_unbounded_ray->set_objective_value(objective_value);
    for (const auto& var : model.vars()) {
        const double value = glp_get_col_prim(m_model, lazy(var).impl());
        m_unbounded_ray->set(var, value);
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
        const double obj = model.get_var_obj(var);
        set_var_attr(index, type, lb, ub, obj);
    }

    // Restore RHS
    for (const auto& ctr : model.ctrs()) {
        const int index = lazy(ctr).impl();
        const int type = model.get_ctr_type(ctr);
        const double rhs = model.get_ctr_rhs(ctr);
        set_ctr_attr(index, type, rhs);
    }

}

void idol::Optimizers::GLPK::save_milp_solution_status() {

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

void idol::Optimizers::GLPK::set_param_time_limit(double t_time_limit) {

    const int value = (int) std::min<double>(std::numeric_limits<int>::max(), std::ceil(t_time_limit) * 1000);
    m_simplex_parameters.tm_lim = value;
    m_mip_parameters.tm_lim = value;

    Optimizer::set_param_time_limit(t_time_limit);
}

void idol::Optimizers::GLPK::set_param_best_obj_stop(double t_best_obj_stop) {
    m_simplex_parameters.obj_ul = t_best_obj_stop;
    Optimizer::set_param_best_obj_stop(t_best_obj_stop);
}

void idol::Optimizers::GLPK::set_param_best_bound_stop(double t_best_bound_stop) {
    m_simplex_parameters.obj_ll = t_best_bound_stop;
    Optimizer::set_param_best_bound_stop(t_best_bound_stop);
}

void idol::Optimizers::GLPK::set_param_presolve(bool t_value) {
    m_simplex_parameters.presolve = t_value ? GLP_MSG_ERR : GLP_MSG_OFF;
    m_mip_parameters.presolve = t_value ? GLP_MSG_ERR : GLP_MSG_OFF;
    Optimizer::set_param_presolve(t_value);
}

idol::SolutionStatus idol::Optimizers::GLPK::get_status() const {
    return m_solution_status;
}

idol::SolutionReason idol::Optimizers::GLPK::get_reason() const {
    return m_solution_reason;
}

double idol::Optimizers::GLPK::get_best_obj() const {
    if (m_solution_status == Unbounded) { return -Inf; }
    if (m_solution_status == Infeasible) { return +Inf; }
    const double constant_term = parent().get_obj_expr().affine().constant();
    return constant_term + (m_solved_as_mip ? glp_mip_obj_val(m_model) : glp_get_obj_val(m_model));
}

double idol::Optimizers::GLPK::get_best_bound() const {
    return get_best_obj();
}

double idol::Optimizers::GLPK::get_var_primal(const Var &t_var) const {
    if (m_solution_status != Feasible && m_solution_status != Optimal) {
        throw Exception("Primal solution not available.");
    }
    const int impl = lazy(t_var).impl();
    return m_solved_as_mip ? glp_mip_col_val(m_model, impl) : glp_get_col_prim(m_model, impl);
}

double idol::Optimizers::GLPK::get_var_ray(const Var &t_var) const {

    if (!m_unbounded_ray.has_value()) {
        throw Exception("Ray not available.");
    }

    return m_unbounded_ray->get(t_var);
}

double idol::Optimizers::GLPK::get_ctr_dual(const Ctr &t_ctr) const {
    if (m_solution_status != Feasible && m_solution_status != Optimal) {
        throw Exception("Primal solution not available.");
    }
    const auto &impl = lazy(t_ctr).impl();
    return glp_get_row_dual(m_model, impl);
}

double idol::Optimizers::GLPK::get_ctr_farkas(const Ctr &t_ctr) const {
    if (!m_farkas_certificate.has_value()) {
        throw Exception("Farkas certificate not available.");
    }

    return m_farkas_certificate->get(t_ctr);
}

double idol::Optimizers::GLPK::get_relative_gap() const {
    return 0;
}

double idol::Optimizers::GLPK::get_absolute_gap() const {
    return 0;
}

unsigned int idol::Optimizers::GLPK::get_n_solutions() const {
    const auto status = get_status();
    return status == Optimal || status == Feasible;
}

unsigned int idol::Optimizers::GLPK::get_solution_index() const {
    return 0;
}

void idol::Optimizers::GLPK::set_solution_index(unsigned int t_index) {
    if (t_index != 0) {
        throw Exception("Solution index out of bounds");
    }
}

idol::Model idol::Optimizers::GLPK::read_from_file(idol::Env &t_env, const std::string &t_filename) {

    const unsigned int size = t_filename.size();

    if (size >= 3 && (t_filename.substr(size - 3) == ".lp") || t_filename.substr(size - 6) == ".lp.gz") {
        return read_from_lp_file(t_env, t_filename);
    }

    if (size >= 3 && (t_filename.substr(size - 4) == ".mps") || t_filename.substr(size - 7) == ".mps.gz") {
        return read_from_mps_file(t_env, t_filename);
    }

    throw Exception("Could not infer file type from file extension.");
}

idol::Model idol::Optimizers::GLPK::read_from_glpk(idol::Env &t_env, glp_prob *t_model) {

    const int n_variables = glp_get_num_cols(t_model);
    const int n_constraints = glp_get_num_rows(t_model);

    Model result(t_env);

    Finally on_terminate([&]() { glp_delete_prob(t_model); });

    for (int j = 1 ; j <= n_variables ; ++j) {

        const double lb = glp_get_col_lb(t_model, j);
        const double ub = glp_get_col_ub(t_model, j);
        const double obj = glp_get_obj_coef(t_model, j);
        VarType type;
        switch (glp_get_col_kind(t_model, j)) {
            case GLP_CV: type = Continuous; break;
            case GLP_IV: type = Integer; break;
            case GLP_BV: type = Binary; break;
            default: throw Exception("Unexpected variable type while parsing.");
        }

        const std::string name = glp_get_col_name(t_model, j);
        result.add_var(lb, ub, type, obj, LinExpr<Ctr>(),name);
    }

    for (int i = 1 ; i <= n_constraints ; ++i) {

        CtrType type;
        double rhs;
        switch (glp_get_row_type(t_model, i)) {
            case GLP_UP:
                type = LessOrEqual;
                rhs = glp_get_row_ub(t_model, i);
                break;
            case GLP_LO:
                type = GreaterOrEqual;
                rhs = glp_get_row_lb(t_model, i);
                break;
            case GLP_FX:
                type = Equal;
                rhs = glp_get_row_ub(t_model, i);
                break;
            default: throw Exception("Unexpected variable type while parsing.");
        }

        const std::string name = glp_get_row_name(t_model, i);
        const int nz = glp_get_mat_row(t_model, i, NULL, NULL);

        auto* indices = new int[nz+1];
        auto* coefficients = new double[nz+1];
        glp_get_mat_row(t_model, i, indices, coefficients);

        LinExpr<Var> lhs;
        for (int k = 1 ; k <= nz ; ++k) {
            const unsigned int var_index = indices[k] - 1;
            const double coefficient = coefficients[k];
            lhs += coefficient * result.get_var_by_index(var_index);
        }

        delete[] indices;
        delete[] coefficients;

        result.add_ctr(std::move(lhs), type, rhs, name);
    }

    if (glp_get_obj_dir(t_model) == GLP_MAX) {
        result.set_obj_sense(Maximize);
    }

    return std::move(result);
}

idol::Model idol::Optimizers::GLPK::read_from_lp_file(idol::Env &t_env, const std::string &t_filename) {

    glp_prob* model = glp_create_prob();
    auto result = glp_read_lp(model,  NULL, t_filename.c_str());
    if (result != 0) {
        throw Exception("Could not parse_variables MPS file.");
    }
    return read_from_glpk(t_env, model);

}

idol::Model idol::Optimizers::GLPK::read_from_mps_file(idol::Env &t_env, const std::string &t_filename) {

    constexpr bool use_fixed_format = false;

    glp_prob *model = glp_create_prob();
    auto result = glp_read_mps(model, use_fixed_format ? GLP_MPS_DECK : GLP_MPS_FILE, NULL, t_filename.c_str());
    if (result != 0) {
        throw Exception("Could not parse_variables MPS file.");
    }
    return read_from_glpk(t_env, model);
}

double idol::Optimizers::GLPK::get_var_reduced_cost(const idol::Var &t_var) const {
    if (m_solution_status != Feasible && m_solution_status != Optimal) {
        throw Exception("Primal solution not available.");
    }
    return glp_get_col_dual(m_model, lazy(t_var).impl());
}

void idol::Optimizers::GLPK::set_param_logs(bool t_value) {
    Optimizer::set_param_logs(t_value);

    if (t_value) {
        m_simplex_parameters.msg_lev = GLP_MSG_ALL;
        m_mip_parameters.msg_lev = GLP_MSG_ALL;
    } else {
        m_simplex_parameters.msg_lev = GLP_MSG_OFF;
        m_mip_parameters.msg_lev = GLP_MSG_OFF;
    }

}

int idol::Optimizers::GLPK::hook_add(const idol::QCtr &t_ctr) {
    throw Exception("Adding quadratic constraints is not implemeted.");
}

void idol::Optimizers::GLPK::hook_remove(const idol::QCtr &t_ctr) {
    throw Exception("Removing quadratic constraints is not implemeted.");
}

int idol::Optimizers::GLPK::hook_add(const idol::SOSCtr &t_ctr) {
    throw Exception("SOS constraints are not supported by GLPK.");
}

void idol::Optimizers::GLPK::hook_remove(const idol::SOSCtr &t_ctr) {
    throw Exception("SOS constraints are not supported by GLPK.");
}

#endif
