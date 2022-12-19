//
// Created by henri on 11/10/22.
//
#include "../../../include/algorithms/solvers/Solvers_GLPK.h"

#ifdef IDOL_USE_GLPK
#include <glpk.h>

Solvers::GLPK::GLPK(Model &t_model) : Solver(t_model) {

    m_model = glp_create_prob();

    glp_set_obj_dir(m_model, t_model.get(Attr::Obj::Sense) == Minimize ? GLP_MIN : GLP_MAX);

    for (const auto& var : t_model.vars()) {
        add_future(var, false);
    }

    for (const auto& ctr : t_model.ctrs()) {
        add_future(ctr);
    }

}

void Solvers::GLPK::execute() {

    update();
    m_solved_as_mip = false;

    glp_smcp parameters;
    glp_init_smcp(&parameters);
    parameters.msg_lev = GLP_MSG_ERR;
    if (const double time_limit = get(Param::Algorithm::TimeLimit) ; !is_pos_inf(time_limit)) {
        parameters.tm_lim = get(Param::Algorithm::TimeLimit) * 1e3;
    }

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
    auto status = this->status();

    if (get(Param::Algorithm::InfeasibleOrUnboundedInfo)) {

        if (status == Infeasible) {
            compute_farkas_certificate();
            return;
        } else if (status == Unbounded) {
            compute_unbounded_ray();
            return;
        }

    }

    if (glp_get_num_int(m_model) > 0 && status == Optimal) {

        glp_iocp parameters_integer;
        glp_init_iocp(&parameters_integer);

        parameters_integer.msg_lev = GLP_MSG_ERR;
        parameters_integer.fp_heur = GLP_OFF;
        parameters_integer.gmi_cuts = GLP_OFF;
        if (const double time_limit = get(Param::Algorithm::TimeLimit) ; !is_pos_inf(time_limit)) {
            parameters_integer.tm_lim = get(Param::Algorithm::TimeLimit) * 1e3;
        }

        glp_intopt(m_model, &parameters_integer);
        m_solved_as_mip = true;

        save_milp_solution_status();

    }

}

void Solvers::GLPK::save_simplex_solution_status() {

    int status = glp_get_status(m_model);

    if (status == GLP_UNDEF) {
        set_status(Unknown);
    } else if (status == GLP_OPT) {
        set_status(Optimal);
    } else if (status == GLP_FEAS) {
        set_status(Feasible);
    } else if (status == GLP_INFEAS || status == GLP_NOFEAS) {
        set_status(Infeasible);
    } else if (status == GLP_UNBND) {
        set_status(Unbounded);
    } else {
        throw Exception("GLPK: Unhandled solution status: " + std::to_string(status));
    }
}

void Solvers::GLPK::save_milp_solution_status() {

    int status = glp_mip_status(m_model);

    if (status == GLP_UNDEF) {
        set_status(Unknown);
    } else if (status == GLP_OPT) {
        set_status(Optimal);
    } else if (status == GLP_FEAS) {
        set_status(Feasible);
    } else if (status == GLP_NOFEAS) {
        set_status(Infeasible);
    } else {
        throw Exception("GLPK: Unhandled solution status: " + std::to_string(status));
    }

}

int Solvers::GLPK::create(const Var &t_var, bool t_with_collaterals) {

    int index;
    if (m_deleted_variables.empty()) {
        index = (int) glp_get_num_cols(m_model) + 1;
        glp_add_cols(m_model, 1);
    } else {
        index = m_deleted_variables.top();
        m_deleted_variables.pop();
    }

    const bool has_lb = !is_neg_inf(get(Attr::Var::Lb, t_var));
    const bool has_ub = !is_pos_inf(get(Attr::Var::Ub, t_var));

    if (has_lb && has_ub) {
        glp_set_col_bnds(m_model, index, GLP_DB, get(Attr::Var::Lb, t_var), get(Attr::Var::Ub, t_var));
    } else if (has_lb) {
        glp_set_col_bnds(m_model, index, GLP_LO, get(Attr::Var::Lb, t_var), 0.);
    } else if (has_ub) {
        glp_set_col_bnds(m_model, index, GLP_UP, 0., get(Attr::Var::Ub, t_var));
    } else {
        glp_set_col_bnds(m_model, index, GLP_FR, 0., 0.);
    }

    glp_set_obj_coef(m_model, index, value(get(Attr::Var::Column, t_var).obj()));

    switch (get(Attr::Var::Type, t_var)) {
        case Continuous: glp_set_col_kind(m_model, index, GLP_CV); break;
        case Binary: glp_set_col_kind(m_model, index, GLP_BV); break;
        case Integer: glp_set_col_kind(m_model, index, GLP_IV); break;
        default: throw std::runtime_error("Unknown variable type.");
    }

    glp_set_col_name(m_model, index, t_var.name().c_str());

    if (t_with_collaterals) {

        const auto n = (int) get(Attr::Var::Column, t_var).linear().size();
        auto* coefficients = new double[n+1];
        auto* indices = new int[n+1];

        int i = 1;
        for (const auto& [ctr, coeff] : get(Attr::Var::Column, t_var).linear()) {
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

int Solvers::GLPK::create(const Ctr &t_ctr, bool t_with_collaterals) {

    int index;
    if (m_deleted_constraints.empty()) {
        index = (int) glp_get_num_rows(m_model) + 1;
        glp_add_rows(m_model, 1);
    } else {
        index = m_deleted_constraints.top();
        m_deleted_constraints.pop();
    }

    switch (model().get(Attr::Ctr::Type, t_ctr)) {
        case LessOrEqual: glp_set_row_bnds(m_model, index, GLP_UP, 0., value(model().get(Attr::Ctr::Row, t_ctr).rhs())); break;
        case GreaterOrEqual: glp_set_row_bnds(m_model, index, GLP_LO, value(model().get(Attr::Ctr::Row, t_ctr).rhs()), 0.); break;
        case Equal: glp_set_row_bnds(m_model, index, GLP_FX, value(model().get(Attr::Ctr::Row, t_ctr).rhs()), 0.); break;
        default: throw std::runtime_error("Unknown constraint type.");
    }

    glp_set_row_name(m_model, index, t_ctr.name().c_str());

    if (t_with_collaterals) {

        const auto n = (int) model().get(Attr::Ctr::Row, t_ctr).linear().size();
        auto* coefficients = new double[n+1];
        auto* indices = new int[n+1];

        int i = 1;
        for (const auto& [var, coeff] : model().get(Attr::Ctr::Row, t_ctr).linear()) {
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

void Solvers::GLPK::compute_farkas_certificate() {

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
        const auto type = model().get(Attr::Ctr::Type, ctr);

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
    for (const auto& [var, constant] : model().get(Attr::Obj::Expr).linear()) {
        glp_set_obj_coef(m_model, future(var).impl(), 0.);
    }

    // Solve feasible model
    glp_smcp parameters;
    glp_init_smcp(&parameters);
    parameters.msg_lev = GLP_MSG_ERR;
    glp_simplex(m_model, &parameters);

    // Save dual values as Farkas certificate
    m_farkas = Solution::Dual();
    double objective_value = value(model().get(Attr::Obj::Const));
    for (const auto& ctr : model().ctrs()) {
        const double dual = glp_get_row_dual(m_model, future(ctr).impl());
        m_farkas->set(ctr, dual);
        objective_value += dual * value(model().get(Attr::Ctr::Row, ctr).rhs());
    }
    m_farkas->set_objective_value(objective_value);

    // Remove added variables
    glp_del_cols(m_model, (int) artificial_variables.size() - 1, artificial_variables.data());

    // Restore objective function
    for (const auto& [var, constant] : model().get(Attr::Obj::Expr).linear()) {
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

void Solvers::GLPK::compute_unbounded_ray() {

    const auto n_variables = (int) model().vars().size();

    std::vector<double> coefficients;
    coefficients.reserve(n_variables + 1);
    coefficients.emplace_back(0.);

    std::vector<int> indices;
    indices.reserve(n_variables + 1);
    indices.emplace_back(0);

    for (const auto& var : model().vars()) {
        if (get(Attr::Var::Lb, var) >= 0.) {
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
        set(Attr::Ctr::Rhs, ctr, 0.);
    }

    // Solve
    glp_smcp parameters;
    glp_init_smcp(&parameters);
    parameters.msg_lev = GLP_MSG_ERR;
    glp_std_basis(m_model);
    glp_simplex(m_model, &parameters);

    // Save ray
    m_ray = Solution::Primal();
    const double objective_value = value(model().get(Attr::Obj::Const)) + glp_get_obj_val(m_model);
    m_ray->set_objective_value(objective_value);
    for (const auto& var : model().vars()) {
        m_ray->set(var, glp_get_col_prim(m_model, future(var).impl()));
    }

    std::cout << "UNBOUNDED RAY LEFT MODEL MODIFIED" << std::endl;
}

void Solvers::GLPK::remove(const Var &t_var, int &t_impl) {
    const int index = future(t_var).impl();

    if (glp_get_col_stat(m_model, index) != GLP_NF) {
        m_rebuild_basis = true;
    }

    glp_set_obj_coef(m_model, index, 0.);
    glp_set_mat_col(m_model, index, 0, NULL, NULL);
    m_deleted_variables.push(index);
}

void Solvers::GLPK::remove(const Ctr &t_ctr, int &t_impl) {
    const int index = future(t_ctr).impl();

    if (glp_get_row_stat(m_model, index) != GLP_NF) {
        m_rebuild_basis = true;
    }

    glp_set_row_bnds(m_model, index, GLP_FX, 0., 0.);
    glp_set_mat_row(m_model, index, 0, NULL, NULL);
    m_deleted_constraints.push(index);
}

Solution::Dual Solvers::GLPK::dual_solution() const {

    if (m_solved_as_mip) {
        throw Exception("Not available for MIPs.");
    }

    Solution::Dual result;
    const auto dual_status = dual(status());
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

    const double objective_value = value(model().get(Attr::Obj::Const)) + glp_get_obj_val(m_model);
    result.set_objective_value(objective_value);

    if (!is_in(result.status(), { Optimal, Feasible })) {
        return result;
    }

    for (const auto& ctr : model().ctrs()) {
        result.set(ctr, glp_get_row_dual(m_model, future(ctr).impl()));
    }

    return result;
}

Solution::Dual Solvers::GLPK::farkas_certificate() const {

    if (status() != Infeasible) {
        throw Exception("Only available for infeasible problems.");
    }

    if (!Algorithm::get(Param::Algorithm::InfeasibleOrUnboundedInfo)) {
        throw Exception("Turn on InfeasibleOrUnboundedInfo before solving your model to access farkas dual information.");
    }

    return m_farkas.value();
}

Solution::Primal Solvers::GLPK::unbounded_ray() const {

    if (status() != Unbounded) {
        throw Exception("Only available for unbounded problems.");
    }

    if (!get(Param::Algorithm::InfeasibleOrUnboundedInfo)) {
        throw Exception("Turn on InfeasibleOrUnboundedInfo before solving your model to access extreme ray information.");
    }

    return m_ray.value();
}

void Solvers::GLPK::update_obj() {
    for (const auto& var : model().vars()) {
        glp_set_obj_coef(m_model, future(var).impl(), 0.);
    }
    for (const auto& [var, constant] : model().get(Attr::Obj::Expr).linear()) {
        glp_set_obj_coef(m_model, future(var).impl(), value(constant));
    }
}

double Solvers::GLPK::objective_value() const {

    const auto status = this->status();

    if (status == Unbounded) {
        return -Inf;
    }

    if (status == Infeasible) {
        return +Inf;
    }

    if (!is_in(status, { Optimal, Feasible })) {
        return 0.;
    }

    const double obj_const = value(model().get(Attr::Obj::Const));

    if (m_solved_as_mip) {
        return obj_const + glp_mip_obj_val(m_model);
    }

    return obj_const + glp_get_obj_val(m_model);
}

Solution::Primal Solvers::GLPK::primal_solution() const {

    Solution::Primal result;
    const auto status = this->status();
    result.set_status(status);
    result.set_objective_value(objective_value());

    if (!is_in(status, { Optimal, Feasible })) {
        return result;
    }

    if (m_solved_as_mip) {

        for (const auto& var : model().vars()) {
            result.set(var, glp_mip_col_val(m_model, future(var).impl()));
        }

    } else {

        for (const auto& var : model().vars()) {
            result.set(var, glp_get_col_prim(m_model, future(var).impl()));
        }

    }

    return result;
}

void Solvers::GLPK::update(const Var &t_var, int &t_impl) {
    std::cout << "SKIPPED UPDATE VAR" << std::endl;
}

void Solvers::GLPK::update(const Ctr &t_ctr, int &t_impl) {
    std::cout << "SKIPPED UPDATE CTR" << std::endl;
}

void Solvers::GLPK::write(const std::string &t_filename) {
    auto filename = std::to_string(n_solved++) + t_filename;
    glp_write_lp(m_model, nullptr, filename.c_str());
}

void Solvers::GLPK::set(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) {

    if (t_attr == Attr::Var::Lb) {

        const bool has_ub = !is_pos_inf(get(Attr::Var::Ub, t_var));
        const int index = future(t_var).impl();

        if (has_ub) {
            if (equals(get(Attr::Var::Ub, t_var), t_value, ToleranceForIntegrality)) {
                glp_set_col_bnds(m_model, index, GLP_FX, t_value, t_value);
            } else {
                glp_set_col_bnds(m_model, index, GLP_DB, t_value, get(Attr::Var::Ub, t_var));
            }
        } else {
            glp_set_col_bnds(m_model, index, GLP_UP, t_value, 0.);
        }

        model().set(Attr::Var::Lb, t_var, t_value);

        return;
    }

    if (t_attr == Attr::Var::Ub) {

        const bool has_lb = !is_neg_inf(get(Attr::Var::Lb, t_var));
        const int index = future(t_var).impl();

        if (has_lb) {
            if (equals(get(Attr::Var::Lb, t_var), t_value, ToleranceForIntegrality)) {
                glp_set_col_bnds(m_model, index, GLP_FX, t_value, t_value);
            } else {
                glp_set_col_bnds(m_model, index, GLP_DB, get(Attr::Var::Lb, t_var), t_value);
            }
        } else {
            glp_set_col_bnds(m_model, index, GLP_UP, 0., t_value);
        }

        model().set(Attr::Var::Ub, t_var, t_value);

        return;
    }

    Delegate::set(t_attr, t_var, t_value);
}

void
Solvers::GLPK::set(const AttributeWithTypeAndArguments<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) {

    if (t_attr == Attr::Ctr::Rhs) {

        if (auto& future = this->future(t_ctr) ; future.has_impl()) {

            const int index = future.impl();

            switch (model().get(Attr::Ctr::Type, t_ctr)) {
                case LessOrEqual:
                    glp_set_row_bnds(m_model, index, GLP_UP, 0., value(t_value));
                    break;
                case GreaterOrEqual:
                    glp_set_row_bnds(m_model, index, GLP_LO, value(t_value), 0.);
                    break;
                case Equal:
                    glp_set_row_bnds(m_model, index, GLP_FX, value(t_value), 0.);
                    break;
                default:
                    throw std::runtime_error("Unknown constraint type.");
            }
        }

        model().set(Attr::Ctr::Rhs, t_ctr, std::move(t_value));

        return;
    }

    Delegate::set(t_attr, t_ctr, t_value);
}

double Solvers::GLPK::get(const AttributeWithTypeAndArguments<double, void> &t_attr) const {

    if (t_attr == Attr::Solution::ObjVal) {
        return objective_value();
    }

    return Delegate::get(t_attr);
}

#endif