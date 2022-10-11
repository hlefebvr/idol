//
// Created by henri on 11/10/22.
//
#include "algorithms/solvers/Solvers_GLPK_Simplex.h"

#ifdef USE_GLPK
#include <glpk.h>

Solvers::GLPK_Simplex::GLPK_Simplex(Model &t_model) : Solver(t_model) {

    m_model = glp_create_prob();

    for (const auto& var : t_model.variables()) {
        auto impl = create_variable_impl_with_objective_coefficient(var);
        add_variable_impl(impl);
    }

    for (const auto& ctr : t_model.constraints()) {
        auto impl = create_constraint_impl_with_rhs(ctr);
        add_constraint_impl(impl);
        set_constraint_lhs(ctr);
    }

}

void Solvers::GLPK_Simplex::execute() {

    glp_smcp parameters;
    glp_init_smcp(&parameters);
    parameters.msg_lev = GLP_MSG_ERR;
    glp_std_basis(m_model);
    std::cout << "WARNING: INITIAL BASIS IS ALWAYS RESTORED HERE" << std::endl;
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

    if (Algorithm::get<Attr::InfeasibleOrUnboundedInfo>()) {
        if (m_solution_status == Infeasible) {
            compute_farkas_certificate();
        } else if (m_solution_status == Unbounded) {
            compute_unbounded_ray();
        }
    }
}

int Solvers::GLPK_Simplex::create_variable_impl_with_objective_coefficient(const Var &t_var) {

    int index;
    if (m_deleted_variables.empty()) {
        index = (int) t_var.index() + 1;
    } else {
        index = m_deleted_variables.top();
        m_deleted_variables.pop();
    }

    glp_add_cols(m_model, 1);

    const bool has_lb = !is_neg_inf(t_var.lb());
    const bool has_ub = !is_pos_inf(t_var.ub());

    if (has_lb && has_ub) {
        glp_set_col_bnds(m_model, index, GLP_DB, t_var.lb(), t_var.ub());
    } else if (has_lb) {
        glp_set_col_bnds(m_model, index, GLP_LO, t_var.lb(), 0.);
    } else if (has_ub) {
        glp_set_col_bnds(m_model, index, GLP_UP, 0., t_var.ub());
    } else {
        glp_set_col_bnds(m_model, index, GLP_FR, 0., 0.);
    }

    glp_set_obj_coef(m_model, index, value(t_var.obj()));

    switch (t_var.type()) {
        case Continuous: glp_set_col_kind(m_model, index, GLP_CV); break;
        case Binary: [[fallthrough]];
        case Integer: throw Exception("GLPK_Simplex is an LP solver. Integer variable encountered. Variable: " + t_var.name() + ".");
        //case Binary: glp_set_col_kind(m_model, index, GLP_BV); break;
        //case Continuous: glp_set_col_kind(m_model, index, GLP_CV); break;
        default: throw std::runtime_error("Unknown variable type.");
    }

    glp_set_col_name(m_model, index, t_var.name().c_str());

    return index;
}

int Solvers::GLPK_Simplex::create_constraint_impl_with_rhs(const Ctr &t_ctr) {

    int index;
    if (m_deleted_constraints.empty()) {
        index = (int) t_ctr.index() + 1;
    } else {
        index = m_deleted_constraints.top();
        m_deleted_constraints.pop();
    }

    glp_add_rows(m_model, 1);

    switch (t_ctr.type()) {
        case LessOrEqual: glp_set_row_bnds(m_model, index, GLP_UP, 0., value(t_ctr.rhs())); break;
        case GreaterOrEqual: glp_set_row_bnds(m_model, index, GLP_LO, value(t_ctr.rhs()), 0.); break;
        case Equal: glp_set_row_bnds(m_model, index, GLP_FX, value(t_ctr.rhs()), 0.); break;
        default: throw std::runtime_error("Unknown constraint type.");
    }

    glp_set_row_name(m_model, index, t_ctr.name().c_str());

    return index;
}

void Solvers::GLPK_Simplex::set_constraint_lhs(const Ctr &t_ctr) {

    const auto n = (int) t_ctr.row().lhs().size();
    auto* coefficients = new double[n+1];
    auto* indices = new int[n+1];

    int i = 1;
    for (const auto& [var, coeff] : t_ctr.row().lhs()) {
        indices[i] = get(var);
        coefficients[i] = value(coeff);
        ++i;
    }

    glp_set_mat_row(m_model, get(t_ctr), n, indices, coefficients);

    delete[] indices;
    delete[] coefficients;
}

void Solvers::GLPK_Simplex::set_variable_components(const Var &t_var) {

    const auto n = (int) t_var.column().components().size();
    auto* coefficients = new double[n+1];
    auto* indices = new int[n+1];

    int i = 1;
    for (const auto& [ctr, coeff] : t_var.column().components()) {
        indices[i] = get(ctr);
        coefficients[i] = value(coeff);
        ++i;
    }

    glp_set_mat_col(m_model, get(t_var), n, indices, coefficients);

    delete[] indices;
    delete[] coefficients;
}

void Solvers::GLPK_Simplex::compute_farkas_certificate() {

    std::list<std::pair<int, int>> basis_rows;
    std::list<std::pair<int, int>> basis_cols;
    for (const auto& ctr : model().constraints()) {
        int index = get(ctr);
        int status = glp_get_row_stat(m_model, index);
        basis_rows.emplace_back(index, status);
    }
    for (const auto& var : model().variables()) {
        int index = get(var);
        int status = glp_get_col_stat(m_model, index);
        basis_cols.emplace_back(index, status);
    }

    std::vector<int> artificial_variables;
    artificial_variables.reserve(2 * model().constraints().size() + 1);
    artificial_variables.emplace_back(0);

    auto* minus_one = new double[2]; minus_one[1] = -1.;
    auto* plus_one = new double[2]; plus_one[1] = 1.;

    // Add artificial variables
    for (const auto& ctr : model().constraints()) {

        auto* index = new int[2];
        index[1] = get(ctr);
        const auto type = ctr.type();

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

    }

    // Set original variables' objective coefficient to zero
    for (const auto& var : model().variables()) {
        glp_set_obj_coef(m_model, get(var), 0.);
    }

    // Solve feasible model
    glp_smcp parameters;
    glp_init_smcp(&parameters);
    parameters.msg_lev = GLP_MSG_ERR;
    glp_simplex(m_model, &parameters);

    // Save dual values as Farkas certificate
    m_farkas = Solution::Dual();
    double objective_value = m_objective_offset;
    for (const auto& ctr : model().constraints()) {
        const double dual = glp_get_row_dual(m_model, get(ctr));
        m_farkas->set(ctr, dual);
        objective_value += dual * value(ctr.rhs());
    }
    m_farkas->set_objective_value(objective_value);

    // Remove added variables
    glp_del_cols(m_model, (int) artificial_variables.size() - 1, artificial_variables.data());

    // Restore objective function
    for (const auto& [var, constant] : model().objective()) {
        glp_set_obj_coef(m_model, get(var), value(constant));
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

    const auto n_variables = (int) model().variables().size();

    std::vector<double> coefficients;
    coefficients.reserve(n_variables + 1);
    coefficients.emplace_back(0.);

    std::vector<int> indices;
    indices.reserve(n_variables + 1);
    indices.emplace_back(0);

    for (const auto& var : model().variables()) {
        if (var.lb() >= 0.) {
            coefficients.emplace_back(1.);
        } else {
            coefficients.emplace_back(-1.);
        }
        coefficients.emplace_back(1.);
        indices.emplace_back(get(var));
    }

    int index = glp_add_rows(m_model, 1);
    glp_set_mat_row(m_model, index, n_variables, indices.data(), coefficients.data());
    glp_set_row_bnds(m_model, index, GLP_LO, 1., 0.);

    for (const auto& ctr : model().constraints()) {
        update_constraint_rhs(ctr, 0.);
    }

    // Solve
    glp_smcp parameters;
    glp_init_smcp(&parameters);
    parameters.msg_lev = GLP_MSG_ERR;
    glp_std_basis(m_model);
    glp_simplex(m_model, &parameters);

    // Save ray
    m_ray = Solution::Primal();
    const double objective_value = m_objective_offset + glp_get_obj_val(m_model);
    m_ray->set_objective_value(objective_value);
    for (const auto& var : model().variables()) {
        m_ray->set(var, glp_get_col_prim(m_model, get(var)));
    }

    std::cout << "UNBOUNDED RAY LEFT MODEL MODIFIED" << std::endl;
}

void Solvers::GLPK_Simplex::update_constraint_rhs(const Ctr &t_ctr, double t_rhs) {

    const int index = get(t_ctr);

    switch (t_ctr.type()) {
        case LessOrEqual: glp_set_row_bnds(m_model, index, GLP_UP, 0., value(t_rhs)); break;
        case GreaterOrEqual: glp_set_row_bnds(m_model, index, GLP_LO, value(t_rhs), 0.); break;
        case Equal: glp_set_row_bnds(m_model, index, GLP_FX, value(t_rhs), 0.); break;
        default: throw std::runtime_error("Unknown constraint type.");
    }

    model().update_rhs(t_ctr, t_rhs);

}

void Solvers::GLPK_Simplex::remove_variable(const Var &t_variable) {
    const int index = get(t_variable);
    glp_set_obj_coef(m_model, index, 0.);
    glp_set_mat_col(m_model, index, 0, NULL, NULL);
    m_deleted_variables.push(index);

    remove_variable_impl(t_variable);
    model().remove(t_variable);
}

void Solvers::GLPK_Simplex::remove_constraint(const Ctr &t_constraint) {
    const int index = get(t_constraint);
    glp_set_row_bnds(m_model, index, GLP_FX, 0., 0.);
    glp_set_mat_row(m_model, index, 0, NULL, NULL);
    m_deleted_constraints.push(index);

    remove_constraint_impl(t_constraint);
    model().remove(t_constraint);
}

Solution::Dual Solvers::GLPK_Simplex::dual_solution() const {

    Solution::Dual result;
    const auto dual_status = dual(m_solution_status.value());
    result.set_status(dual_status);

    if (dual_status == Unbounded) {
        result.set_objective_value(+Inf);
        return result;
    }

    if (is_in(dual_status, { Infeasible, InfeasibleTimeLimit })) {
        result.set_objective_value(-Inf);
        return result;
    }

    if (!is_in(result.status(), { Optimal, Feasible, FeasibleTimeLimit })) {
        result.set_objective_value(0.);
        return result;
    }

    const double objective_value = m_objective_offset + glp_get_obj_val(m_model);
    result.set_objective_value(objective_value);

    if (!is_in(result.status(), { Optimal, Feasible, FeasibleTimeLimit })) {
        return result;
    }

    for (const auto& ctr : model().constraints()) {
        result.set(ctr, glp_get_row_dual(m_model, get(ctr)));
    }

    return result;
}

Solution::Dual Solvers::GLPK_Simplex::farkas_certificate() const {

    if (m_solution_status != Infeasible) {
        throw Exception("Only available for infeasible problems.");
    }

    if (!Algorithm::get<Attr::InfeasibleOrUnboundedInfo>()) {
        throw Exception("Turn on infeasible_or_unbounded_info before solving your model to access farkas dual information.");
    }

    return m_farkas.value();
}

void Solvers::GLPK_Simplex::update_objective(const Row &t_objective) {
    m_objective_offset = value(t_objective.rhs());
    // TODO this must be better done after refacto of Objective and Rhs
    for (const auto& var : model().variables()) {
        glp_set_obj_coef(m_model, get(var), value(t_objective.lhs().get(var)));
    }
    model().update_objective(t_objective);
}

Solution::Primal Solvers::GLPK_Simplex::primal_solution() const {

    Solution::Primal result;
    const auto status = m_solution_status.value();
    result.set_status(status);

    if (status == Unbounded) {
        result.set_objective_value(-Inf);
        return result;
    }

    if (is_in(status, { Infeasible, InfeasibleTimeLimit })) {
        result.set_objective_value(+Inf);
        return result;
    }

    if (!is_in(result.status(), { Optimal, Feasible, FeasibleTimeLimit })) {
        result.set_objective_value(0.);
        return result;
    }

    const double objective_value = m_objective_offset + glp_get_obj_val(m_model);
    result.set_objective_value(objective_value);

    for (const auto& var : model().variables()) {
        result.set(var, glp_get_col_prim(m_model, get(var)));
    }

    return result;
}

Var Solvers::GLPK_Simplex::add_column(TempVar t_temporary_variable) {
    auto var = model().add_variable(std::move(t_temporary_variable));
    auto impl = create_variable_impl_with_objective_coefficient(var);
    add_variable_impl(impl);
    set_variable_components(var);
    return var;
}

Ctr Solvers::GLPK_Simplex::add_constraint(TempCtr t_temporary_constraint) {
    auto ctr = model().add_constraint(std::move(t_temporary_constraint));
    auto impl = create_constraint_impl_with_rhs(ctr);
    add_constraint_impl(impl);
    set_constraint_lhs(ctr);
    return ctr;
}

void Solvers::GLPK_Simplex::set_lower_bound(const Var &t_var, double t_lb) {

    const bool has_ub = !is_pos_inf(t_var.ub());
    const int index = get(t_var);

    if (has_ub) {
        if (equals(t_var.ub(), t_lb, ToleranceForIntegrality)) {
            glp_set_col_bnds(m_model, index, GLP_FX, t_lb, t_lb);
        } else {
            glp_set_col_bnds(m_model, index, GLP_DB, t_lb, t_var.ub());
        }
    } else {
        glp_set_col_bnds(m_model, index, GLP_UP, t_lb, 0.);
    }

    model().update_lb(t_var, t_lb);
}

void Solvers::GLPK_Simplex::set_upper_bound(const Var &t_var, double t_ub) {

    const bool has_lb = !is_neg_inf(t_var.lb());
    const int index = get(t_var);

    if (has_lb) {
        if (equals(t_var.lb(), t_ub, ToleranceForIntegrality)) {
            glp_set_col_bnds(m_model, index, GLP_FX, t_ub, t_ub);
        } else {
            glp_set_col_bnds(m_model, index, GLP_DB, t_var.lb(), t_ub);
        }
    } else {
        glp_set_col_bnds(m_model, index, GLP_UP, 0., t_ub);
    }

    model().update_ub(t_var, t_ub);
}

#endif