//
// Created by henri on 11/10/22.
//
#include "../../../include/algorithms/solvers/Solvers_GLPK_Simplex.h"

#ifdef IDOL_USE_GLPK
#include <glpk.h>

Solvers::GLPK_Simplex::GLPK_Simplex(Model &t_model) : Solver(t_model) {

    m_model = glp_create_prob();

    glp_set_obj_dir(m_model, t_model.sense() == Minimize ? GLP_MIN : GLP_MAX);

    for (const auto& var : t_model.vars()) {
        add_future(var, false);
    }

    for (const auto& ctr : t_model.ctrs()) {
        add_future(ctr);
    }

}

void Solvers::GLPK_Simplex::execute() {

    update();

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

    int status = glp_get_status(m_model);

    if (status == GLP_UNDEF) {
        m_solution_status = Unknown;
    } else if (status == GLP_OPT) {
        m_solution_status = Optimal;
    } else if (status == GLP_FEAS) {
        m_solution_status = Feasible;
    } else if (status == GLP_INFEAS || status == GLP_NOFEAS) {
        m_solution_status = Infeasible;
    } else if (status == GLP_UNBND) {
        m_solution_status = Unbounded;
    }

    if (get<Attr::InfeasibleOrUnboundedInfo>()) {
        if (m_solution_status == Infeasible) {
            compute_farkas_certificate();
        } else if (m_solution_status == Unbounded) {
            compute_unbounded_ray();
        }
    }

}

int Solvers::GLPK_Simplex::create(const Var &t_var, bool t_with_collaterals) {

    int index;
    if (m_deleted_variables.empty()) {
        index = (int) glp_get_num_cols(m_model) + 1;
        glp_add_cols(m_model, 1);
    } else {
        index = m_deleted_variables.top();
        m_deleted_variables.pop();
    }

    const bool has_lb = !is_neg_inf(get_lb(t_var));
    const bool has_ub = !is_pos_inf(get_ub(t_var));

    if (has_lb && has_ub) {
        glp_set_col_bnds(m_model, index, GLP_DB, get_lb(t_var), get_ub(t_var));
    } else if (has_lb) {
        glp_set_col_bnds(m_model, index, GLP_LO, get_lb(t_var), 0.);
    } else if (has_ub) {
        glp_set_col_bnds(m_model, index, GLP_UP, 0., get_ub(t_var));
    } else {
        glp_set_col_bnds(m_model, index, GLP_FR, 0., 0.);
    }

    glp_set_obj_coef(m_model, index, value(get_column(t_var).objective_coefficient()));

    switch (get_type(t_var)) {
        case Continuous: glp_set_col_kind(m_model, index, GLP_CV); break;
        case Binary: [[fallthrough]];
        case Integer: throw Exception("GLPK_Simplex is an LP solver. Integer variable encountered. Variable: " + t_var.name() + ".");
            //case Binary: glp_set_col_kind(m_model, index, GLP_BV); break;
            //case Continuous: glp_set_col_kind(m_model, index, GLP_CV); break;
        default: throw std::runtime_error("Unknown variable type.");
    }

    glp_set_col_name(m_model, index, t_var.name().c_str());

    if (t_with_collaterals) {

        const auto n = (int) get_column(t_var).components().linear().size();
        auto* coefficients = new double[n+1];
        auto* indices = new int[n+1];

        int i = 1;
        for (const auto& [ctr, coeff] : get_column(t_var).components().linear()) {
            indices[i] = future(ctr).impl();
            coefficients[i] = value(coeff);
            ++i;
        }

        glp_set_mat_col(m_model, index, n, indices, coefficients);

        delete[] indices;
        delete[] coefficients;

    }

    return index;
}

int Solvers::GLPK_Simplex::create(const Ctr &t_ctr, bool t_with_collaterals) {

    int index;
    if (m_deleted_constraints.empty()) {
        index = (int) glp_get_num_rows(m_model) + 1;
        glp_add_rows(m_model, 1);
    } else {
        index = m_deleted_constraints.top();
        m_deleted_constraints.pop();
    }

    switch (model().get_type(t_ctr)) {
        case LessOrEqual: glp_set_row_bnds(m_model, index, GLP_UP, 0., value(model().get_row(t_ctr).rhs())); break;
        case GreaterOrEqual: glp_set_row_bnds(m_model, index, GLP_LO, value(model().get_row(t_ctr).rhs()), 0.); break;
        case Equal: glp_set_row_bnds(m_model, index, GLP_FX, value(model().get_row(t_ctr).rhs()), 0.); break;
        default: throw std::runtime_error("Unknown constraint type.");
    }

    glp_set_row_name(m_model, index, t_ctr.name().c_str());

    if (t_with_collaterals) {

        const auto n = (int) model().get_row(t_ctr).lhs().linear().size();
        auto* coefficients = new double[n+1];
        auto* indices = new int[n+1];

        int i = 1;
        for (const auto& [var, coeff] : model().get_row(t_ctr).lhs().linear()) {
            indices[i] = future(var).impl();
            coefficients[i] = value(coeff);
            ++i;
        }

        glp_set_mat_row(m_model, index, n, indices, coefficients);

        delete[] indices;
        delete[] coefficients;
    }

    return index;
}

void Solvers::GLPK_Simplex::compute_farkas_certificate() {

    std::list<std::pair<int, int>> basis_rows;
    std::list<std::pair<int, int>> basis_cols;
    for (const auto& ctr : model().ctrs()) {
        int index = future(ctr).impl();
        int status = glp_get_row_stat(m_model, index);
        basis_rows.emplace_back(index, status);
    }
    for (const auto& var : model().vars()) {
        int index = future(var).impl();
        int status = glp_get_col_stat(m_model, index);
        basis_cols.emplace_back(index, status);
    }

    std::vector<int> artificial_variables;
    artificial_variables.reserve(2 * model().ctrs().size() + 1);
    artificial_variables.emplace_back(0);

    auto* minus_one = new double[2]; minus_one[1] = -1.;
    auto* plus_one = new double[2]; plus_one[1] = 1.;

    // Add artificial variables
    for (const auto& ctr : model().ctrs()) {

        auto* index = new int[2];
        index[1] = future(ctr).impl();
        const auto type = model().get_type(ctr);

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
    for (const auto& [var, constant] : model().obj().linear()) {
        glp_set_obj_coef(m_model, future(var).impl(), 0.);
    }

    // Solve feasible model
    glp_smcp parameters;
    glp_init_smcp(&parameters);
    parameters.msg_lev = GLP_MSG_ERR;
    glp_simplex(m_model, &parameters);

    // Save dual values as Farkas certificate
    m_farkas = Solution::Dual();
    double objective_value = value(model().obj().constant());
    for (const auto& ctr : model().ctrs()) {
        const double dual = glp_get_row_dual(m_model, future(ctr).impl());
        m_farkas->set(ctr, dual);
        objective_value += dual * value(model().get_row(ctr).rhs());
    }
    m_farkas->set_objective_value(objective_value);

    // Remove added variables
    glp_del_cols(m_model, (int) artificial_variables.size() - 1, artificial_variables.data());

    // Restore objective function
    for (const auto& [var, constant] : model().obj().linear()) {
        glp_set_obj_coef(m_model, future(var).impl(), value(constant));
    }

    // Restore basis
    for (const auto& [index, status] : basis_rows) {
        glp_set_row_stat(m_model, index, status);
    }
    for (const auto& [index, status] : basis_cols) {
        glp_set_col_stat(m_model, index, status);
    }

}

void Solvers::GLPK_Simplex::compute_unbounded_ray() {

    const auto n_variables = (int) model().vars().size();

    std::vector<double> coefficients;
    coefficients.reserve(n_variables + 1);
    coefficients.emplace_back(0.);

    std::vector<int> indices;
    indices.reserve(n_variables + 1);
    indices.emplace_back(0);

    for (const auto& var : model().vars()) {
        if (get_lb(var) >= 0.) {
            coefficients.emplace_back(1.);
        } else {
            coefficients.emplace_back(-1.);
        }
        coefficients.emplace_back(1.);
        indices.emplace_back(future(var).impl());
    }

    int index = glp_add_rows(m_model, 1);
    glp_set_mat_row(m_model, index, n_variables, indices.data(), coefficients.data());
    glp_set_row_bnds(m_model, index, GLP_LO, 1., 0.);

    for (const auto& ctr : model().ctrs()) {
        update_rhs_coeff(ctr, 0.);
    }

    // Solve
    glp_smcp parameters;
    glp_init_smcp(&parameters);
    parameters.msg_lev = GLP_MSG_ERR;
    glp_std_basis(m_model);
    glp_simplex(m_model, &parameters);

    // Save ray
    m_ray = Solution::Primal();
    const double objective_value = value(model().obj().constant()) + glp_get_obj_val(m_model);
    m_ray->set_objective_value(objective_value);
    for (const auto& var : model().vars()) {
        m_ray->set(var, glp_get_col_prim(m_model, future(var).impl()));
    }

    std::cout << "UNBOUNDED RAY LEFT MODEL MODIFIED" << std::endl;
}

void Solvers::GLPK_Simplex::update_rhs_coeff(const Ctr &t_ctr, double t_rhs) {

    const int index = future(t_ctr).impl();

    switch (model().get_type(t_ctr)) {
        case LessOrEqual: glp_set_row_bnds(m_model, index, GLP_UP, 0., value(t_rhs)); break;
        case GreaterOrEqual: glp_set_row_bnds(m_model, index, GLP_LO, value(t_rhs), 0.); break;
        case Equal: glp_set_row_bnds(m_model, index, GLP_FX, value(t_rhs), 0.); break;
        default: throw std::runtime_error("Unknown constraint type.");
    }

    model().update_rhs_coeff(t_ctr, t_rhs);

}

void Solvers::GLPK_Simplex::remove(const Var &t_var, int &t_impl) {
    const int index = future(t_var).impl();

    if (glp_get_col_stat(m_model, index) != GLP_NF) {
        m_rebuild_basis = true;
    }

    glp_set_obj_coef(m_model, index, 0.);
    glp_set_mat_col(m_model, index, 0, NULL, NULL);
    m_deleted_variables.push(index);
}

void Solvers::GLPK_Simplex::remove(const Ctr &t_ctr, int &t_impl) {
    const int index = future(t_ctr).impl();

    if (glp_get_row_stat(m_model, index) != GLP_NF) {
        m_rebuild_basis = true;
    }

    glp_set_row_bnds(m_model, index, GLP_FX, 0., 0.);
    glp_set_mat_row(m_model, index, 0, NULL, NULL);
    m_deleted_constraints.push(index);
}

Solution::Dual Solvers::GLPK_Simplex::dual_solution() const {

    Solution::Dual result;
    const auto dual_status = dual(m_solution_status.value());
    result.set_status(dual_status);

    if (dual_status == Unbounded) {
        result.set_objective_value(+Inf);
        return result;
    }

    if (dual_status == Infeasible) {
        result.set_objective_value(-Inf);
        return result;
    }

    if (!is_in(result.status(), { Optimal, Feasible })) {
        result.set_objective_value(0.);
        return result;
    }

    const double objective_value = value(model().obj().constant()) + glp_get_obj_val(m_model);
    result.set_objective_value(objective_value);

    if (!is_in(result.status(), { Optimal, Feasible })) {
        return result;
    }

    for (const auto& ctr : model().ctrs()) {
        result.set(ctr, glp_get_row_dual(m_model, future(ctr).impl()));
    }

    return result;
}

Solution::Dual Solvers::GLPK_Simplex::farkas_certificate() const {

    if (m_solution_status != Infeasible) {
        throw Exception("Only available for infeasible problems.");
    }

    if (!Algorithm::get<Attr::InfeasibleOrUnboundedInfo>()) {
        throw Exception("Turn on InfeasibleOrUnboundedInfo before solving your model to access farkas dual information.");
    }

    return m_farkas.value();
}

Solution::Primal Solvers::GLPK_Simplex::unbounded_ray() const {

    if (m_solution_status != Unbounded) {
        throw Exception("Only available for unbounded problems.");
    }

    if (!get<Attr::InfeasibleOrUnboundedInfo>()) {
        throw Exception("Turn on InfeasibleOrUnboundedInfo before solving your model to access extreme ray information.");
    }

    return m_ray.value();
}

void Solvers::GLPK_Simplex::update_obj() {
    for (const auto& var : model().vars()) {
        glp_set_obj_coef(m_model, future(var).impl(), 0.);
    }
    for (const auto& [var, constant] : model().obj().linear()) {
        glp_set_obj_coef(m_model, future(var).impl(), value(constant));
    }
}


Solution::Primal Solvers::GLPK_Simplex::primal_solution() const {

    Solution::Primal result;
    const auto status = m_solution_status.value();
    result.set_status(status);

    if (status == Unbounded) {
        result.set_objective_value(-Inf);
        return result;
    }

    if (status == Infeasible) {
        result.set_objective_value(+Inf);
        return result;
    }

    if (!is_in(result.status(), { Optimal, Feasible })) {
        result.set_objective_value(0.);
        return result;
    }

    const double objective_value = value(model().obj().constant()) + glp_get_obj_val(m_model);
    result.set_objective_value(objective_value);

    for (const auto& var : model().vars()) {
        result.set(var, glp_get_col_prim(m_model, future(var).impl()));
    }

    return result;
}

void Solvers::GLPK_Simplex::update_var_lb(const Var &t_var, double t_lb) {

    const bool has_ub = !is_pos_inf(get_ub(t_var));
    const int index = future(t_var).impl();

    if (has_ub) {
        if (equals(get_ub(t_var), t_lb, ToleranceForIntegrality)) {
            glp_set_col_bnds(m_model, index, GLP_FX, t_lb, t_lb);
        } else {
            glp_set_col_bnds(m_model, index, GLP_DB, t_lb, get_ub(t_var));
        }
    } else {
        glp_set_col_bnds(m_model, index, GLP_UP, t_lb, 0.);
    }

    model().update_var_lb(t_var, t_lb);
}

void Solvers::GLPK_Simplex::update_var_ub(const Var &t_var, double t_ub) {

    const bool has_lb = !is_neg_inf(get_lb(t_var));
    const int index = future(t_var).impl();

    if (has_lb) {
        if (equals(get_lb(t_var), t_ub, ToleranceForIntegrality)) {
            glp_set_col_bnds(m_model, index, GLP_FX, t_ub, t_ub);
        } else {
            glp_set_col_bnds(m_model, index, GLP_DB, get_lb(t_var), t_ub);
        }
    } else {
        glp_set_col_bnds(m_model, index, GLP_UP, 0., t_ub);
    }

    model().update_var_ub(t_var, t_ub);
}

void Solvers::GLPK_Simplex::update(const Var &t_var, int &t_impl) {
    std::cout << "SKIPPED UPDATE VAR" << std::endl;
}

void Solvers::GLPK_Simplex::update(const Ctr &t_ctr, int &t_impl) {
    std::cout << "SKIPPED UPDATE CTR" << std::endl;
}

void Solvers::GLPK_Simplex::write(const std::string &t_filename) {
    auto filename = std::to_string(n_solved++) + t_filename;
    glp_write_lp(m_model, nullptr, filename.c_str());
}

#endif