//
// Created by henri on 11/10/22.
//
#include "solvers/glpk/GLPK.h"
#include <glpk.h>

GLPK::GLPK(Model &t_model) : BaseSolver<int, int>(t_model) {

    m_model = glp_create_prob();

    glp_set_obj_dir(m_model, GLP_MIN);

    init_model(t_model);

}

GLPK::~GLPK() {
    //glp_delete_prob(m_model);
    glp_free_env();
}

int GLPK::create_variable(const Var &t_var) {

    int index;
    if (m_free_columns.empty()) {
        index = (int) t_var.index() + 1;
    } else {
        index = m_free_columns.top();
        m_free_columns.pop();
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
        case Integer: glp_set_col_kind(m_model, index, GLP_IV); break;
        case Binary: glp_set_col_kind(m_model, index, GLP_BV); break;
        case Continuous: glp_set_col_kind(m_model, index, GLP_CV); break;
        default: throw std::runtime_error("Unknown variable type.");
    }

    glp_set_col_name(m_model, index, t_var.name().c_str());

    return index;
}

int GLPK::create_constraint(const Ctr &t_ctr) {

    int index;
    if (m_free_constraints.empty()) {
        index = (int) t_ctr.index() + 1;
    } else {
        index = m_free_constraints.top();
        m_free_constraints.pop();
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

void GLPK::fill_column(const Var &t_var) {

    set_objective_coefficient(t_var, t_var.obj());

    const unsigned int n = t_var.column().components().size();
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

void GLPK::fill_row(const Ctr &t_ctr) {

    set_rhs(t_ctr, t_ctr.rhs());

    const unsigned int n = t_ctr.row().lhs().size();
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

void GLPK::remove_variable(const Var &t_var) {
    const int index = get(t_var);
    glp_set_obj_coef(m_model, index, 0.);
    glp_set_mat_col(m_model, index, 0, NULL, NULL);
    m_free_columns.push(index);
}

void GLPK::remove_constraint(const Ctr &t_ctr) {
    const int index = get(t_ctr);
    glp_set_row_bnds(m_model, index, GLP_FX, 0., 0.);
    glp_set_mat_row(m_model, index, 0, NULL, NULL);
    m_free_constraints.push(index);
}

void GLPK::set_objective_coefficient(const Var &t_var, const Constant &t_coeff) {
    glp_set_obj_coef(m_model, get(t_var), value(t_coeff));
}

void GLPK::set_objective_offset(const Constant &t_offset) {
    m_objective_offset = value(t_offset);
}

void GLPK::set_rhs(const Ctr &t_ctr, const Constant &t_coeff) {

    const int index = get(t_ctr);

    switch (t_ctr.type()) {
        case LessOrEqual: glp_set_row_bnds(m_model, index, GLP_UP, 0., value(t_ctr.rhs())); break;
        case GreaterOrEqual: glp_set_row_bnds(m_model, index, GLP_LO, value(t_ctr.rhs()), 0.); break;
        case Equal: glp_set_row_bnds(m_model, index, GLP_FX, value(t_ctr.rhs()), 0.); break;
        default: throw std::runtime_error("Unknown constraint type.");
    }

}

void GLPK::set_coefficient(const Ctr &t_ctr, const Var &t_var, const Constant &t_coefficient) {
    throw std::runtime_error("Not implemented");
}

void GLPK::set_lb(const Var &t_var, double t_lb) {

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

}

void GLPK::set_ub(const Var &t_var, double t_ub) {

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

}

void GLPK::set_type(const Var &t_var, VarType t_type) {
    throw Exception("Not implemented");
}

void GLPK::set_type(const Ctr &t_ctr, CtrType t_type) {
    throw Exception("Not implemented");
}

SolutionStatus GLPK::get_primal_status() const {
    return m_solution_status.value();
}

SolutionStatus GLPK::get_dual_status() const {
    return dual(get_primal_status());
}

double GLPK::get_primal_objective_value() const {
    return m_objective_offset + glp_get_obj_val(m_model);
}

double GLPK::get_dual_objective_value() const {
    return get_primal_objective_value();
}

double GLPK::get_primal_value(const Var &t_var) const {
    return glp_get_col_prim(m_model, get(t_var));
}

double GLPK::get_unbounded_ray(const Var &t_var) const {
    return m_ray->get(t_var);
}

double GLPK::get_unbounded_ray_objective_value() const {
    return m_ray->objective_value();
}

double GLPK::get_dual_value(const Ctr &t_ctr) const {
    return glp_get_row_dual(m_model, get(t_ctr));
}

double GLPK::get_dual_farkas_objective_value() const {
    double result = 0.;
    for (const auto& ctr : source_model().constraints()) {
        result += get_dual_farkas_value(ctr) * value(ctr.rhs());
    }
    return result;
}

double GLPK::get_dual_farkas_value(const Ctr &t_ctr) const {
    return m_farkas->get(t_ctr);
}

void GLPK::write(const std::string &t_filename) {
    glp_write_lp(m_model, NULL, t_filename.c_str());
}

void GLPK::solve() {

    glp_smcp parameters;
    glp_init_smcp(&parameters);
    parameters.msg_lev = GLP_MSG_ERR;
    glp_std_basis(m_model);
    std::cout << "WARNIGN: INITIAL BASIS IS ALWAYS RESTORED HERE" << std::endl;
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

    if (infeasible_or_unbounded_info()) {
        if (m_solution_status == Infeasible) {
            compute_dual_farkas();
        } else if (m_solution_status == Unbounded) {
            compute_unbounded_ray();
        }
    }
}

void GLPK::set_infeasible_or_unbounded_info(bool t_value) {
    m_infeas_or_unbounded_info = t_value;
}

bool GLPK::infeasible_or_unbounded_info() const {
    return m_infeas_or_unbounded_info;
}

void GLPK::set_algorithm_for_lp(AlgorithmForLP t_algorithm) {

}

void GLPK::set_presolve(bool t_value) {

}

bool GLPK::presolve() const {
    return true;
}

AlgorithmForLP GLPK::algorithm_for_lp() const {
    return Automatic;
}

void GLPK::compute_iis() {
    throw Exception("Not implemented");
}

void GLPK::compute_dual_farkas() {

    std::list<std::pair<int, int>> basis_rows;
    std::list<std::pair<int, int>> basis_cols;
    for (const auto& ctr : source_model().constraints()) {
        int index = get(ctr);
        int status = glp_get_row_stat(m_model, index);
        basis_rows.emplace_back(index, status);
    }
    for (const auto& var : source_model().variables()) {
        int index = get(var);
        int status = glp_get_col_stat(m_model, index);
        basis_cols.emplace_back(index, status);
    }

    std::vector<int> artificial_variables;
    artificial_variables.reserve(2 * source_model().constraints().size() + 1);
    artificial_variables.emplace_back(0);

    auto* minus_one = new double[2]; minus_one[1] = -1.;
    auto* plus_one = new double[2]; plus_one[1] = 1.;

    // Add artificial variables
    for (const auto& ctr : source_model().constraints()) {

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
    for (const auto& var : source_model().variables()) {
        glp_set_obj_coef(m_model, get(var), 0.);
    }

    // Solve feasible model
    glp_smcp parameters;
    glp_init_smcp(&parameters);
    parameters.msg_lev = GLP_MSG_ERR;
    glp_simplex(m_model, &parameters);

    // Save dual values as Farkas certificate
    m_farkas = Solution::Dual();
    m_farkas->set_status(algorithm_for_lp() == DualSimplex ? Optimal : Feasible);
    m_farkas->set_objective_value(get_dual_farkas_objective_value());
    for (const auto& ctr : source_model().constraints()) {
        m_farkas->set(ctr, glp_get_row_dual(m_model, get(ctr)));
    }

    // Remove added variables
    glp_del_cols(m_model, (int) artificial_variables.size() - 1, artificial_variables.data());

    // Restore objective function
    for (const auto& [var, constant] : source_model().objective()) {
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

void GLPK::compute_unbounded_ray() {

    const auto n_variables = (int) source_model().variables().size();

    std::vector<double> coefficients;
    coefficients.reserve(n_variables + 1);
    coefficients.emplace_back(0.);

    std::vector<int> indices;
    indices.reserve(n_variables + 1);
    indices.emplace_back(0);

    for (const auto& var : source_model().variables()) {
        if (var.lb() >= 0.) {
            coefficients.emplace_back(1.);
        } else {
            coefficients.emplace_back(-1.);
       }
        indices.emplace_back(get(var));
    }

    int index = glp_add_rows(m_model, 1);
    glp_set_mat_row(m_model, index, n_variables, indices.data(), coefficients.data());
    glp_set_row_bnds(m_model, index, GLP_LO, 1., 0.);

    for (const auto& ctr : source_model().constraints()) {
        set_rhs(ctr, 0.);
    }

    // Solve
    glp_smcp parameters;
    glp_init_smcp(&parameters);
    parameters.msg_lev = GLP_MSG_ERR;
    glp_std_basis(m_model);
    glp_simplex(m_model, &parameters);

    // Save ray
    m_ray = Solution::Primal();
    m_ray->set_status(algorithm_for_lp() == PrimalSimplex ? Optimal : Feasible);
    m_ray->set_objective_value(glp_get_obj_val(m_model));
    for (const auto& var : source_model().variables()) {
        m_ray->set(var, glp_get_col_prim(m_model, get(var)));
    }

    std::cout << "UNBOUNDED RAY LEFT MODEL MODIFIED" << std::endl;

}
