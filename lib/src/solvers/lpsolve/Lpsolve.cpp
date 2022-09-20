//
// Created by henri on 10/09/22.
//
#include "solvers/lpsolve/Lpsolve.h"
#ifdef USE_LPSOLVE
#include <lp_lib.h>

void Lpsolve::throw_if_error(unsigned char t_code, const std::string& t_msg) {
    if (!t_code) {
        throw std::runtime_error("Lpsolve error: " + t_msg);
    }
}

void Lpsolve::throw_if_error(void* t_ptr, const std::string& t_msg) {
    if (!t_ptr) {
        throw std::runtime_error("Lpsolve error: " + t_msg);
    }
}

Lpsolve::Lpsolve(Model& t_model) : BaseSolver<int, int>(t_model) {

    const auto n_rows = (int) t_model.constraints().size();
    const auto n_cols = (int) t_model.constraints().size();
    model = make_lp(0, 0);
    throw_if_error(model, "could not create model");
    auto result = resize_lp(model, n_rows, n_cols);
    throw_if_error(result, "could not resize model");

    set_verbose(model, NEUTRAL);

    set_minim(model);

    init_model(t_model);

    std::cout << "IMPORTANT WARNING: MANUALLY SETTING set_infeasible_or_unbounded_info TO TRUE." << std::endl;
    set_infeasible_or_unbounded_info(true);
}

Lpsolve::~Lpsolve() {
    free_lp(&model);
}

void Lpsolve::write(const std::string &t_filename) {
    write_lp(model, (char*) t_filename.c_str());
}

SolutionStatus convert_lpsolve_status(int t_lpsolve_status) {
    switch (t_lpsolve_status) {
        case NUMFAILURE: [[fallthrough]];
        case NOMEMORY: return Fail;
        case PRESOLVED: [[fallthrough]];
        case OPTIMAL: return Optimal;
        case SUBOPTIMAL: return FeasibleTimeLimit;
        case INFEASIBLE: return Infeasible;
        case UNBOUNDED: return Unbounded;
        case TIMEOUT: return InfeasibleTimeLimit;
        default:;
    }
    throw std::runtime_error("Did not know what to do with lpsolve status: " + std::to_string(t_lpsolve_status));
}

void Lpsolve::solve() {

    set_infinite(model, Inf);

    if (source_model().variables().size() > 0) {

        int status = ::solve(model);
        m_solution_status = convert_lpsolve_status(status);

    } else {

        m_solution_status = Infeasible;

    }

    if (infeasible_or_unbounded_info()) {

        if (m_solution_status.value() == Unbounded) {
            compute_unbounded_ray();
        } else if (m_solution_status.value() == Infeasible) {
            compute_farkas_dual();
        }

    }

}

int Lpsolve::create_variable(const Var &t_var) {

    unsigned char success;

    int index;
    if (m_free_columns.empty()) {
        index = (int) t_var.index() + 1;

        double coeff = value(t_var.obj());
        int row_id = 0;

        success = add_columnex(model, 1, &coeff, &row_id);
        throw_if_error(success, "could not add column");
    } else {
        index = m_free_columns.top();
        m_free_columns.pop();
    }

    switch (t_var.type()) {
        case Integer: success = set_int(model, index, true); break;
        case Binary: success = set_binary(model, index, true); break;
        default: success = set_unbounded(model, index); break;
    }
    throw_if_error(success, "could not set variable type");

    if (double lb = t_var.lb() ; !is_neg_inf(lb)) {
        success = set_lowbo(model, index, t_var.lb());
        throw_if_error(success, "could not set LB");
    }

    if (double ub = t_var.ub() ; !is_pos_inf(ub)) {
        success = set_upbo(model, index, t_var.ub());
        throw_if_error(success, "could not set UB");
    }

    success = set_col_name(model, index, (char*) t_var.name().c_str());
    throw_if_error(success, "could not set variable name");

    return index;
}

int Lpsolve::create_constraint(const Ctr &t_ctr) {

    unsigned char success;

    int type;
    switch (t_ctr.type()) {
        case LessOrEqual: type = LE; break;
        case GreaterOrEqual: type = GE; break;
        case Equal: type = EQ; break;
        default: throw std::runtime_error("Unexpected constraint type: " + std::to_string(t_ctr.type()));
    }

    int index;
    if (!m_free_constraints.empty()) {

        index = m_free_constraints.top();
        m_free_constraints.pop();

        success = set_constr_type(model, index, type);
        throw_if_error(success, "could not change constraint type");

    } else {

        index = (int) t_ctr.index() + 1;

        double coeff = value(t_ctr.rhs());
        int rhs_index = 0;
        double zero = 0.;

        //success = set_add_rowmode(model, true);
        //throw_if_error(success, "could not enter rowmode");

        success = add_constraintex(model, 1, &zero, &rhs_index, type, coeff);
        throw_if_error(success, "could not add constraint");

        //success = set_add_rowmode(model, false);
        //throw_if_error(success, "could not exit rowmode");
    }

    success = set_row_name(model, index, (char*) t_ctr.name().c_str());
    throw_if_error(success, "could not set constraint name");
    
    return index;
}

void Lpsolve::fill_column(const Var &t_var) {
    if (t_var.is_virtual()) { return; }
    set_objective_coefficient(t_var, t_var.obj());
    for (const auto& [ctr, coeff] : t_var.column()) {
        set_coefficient(ctr, t_var, coeff);
    }
}

void Lpsolve::fill_row(const Ctr &t_ctr) {
    set_rhs(t_ctr, t_ctr.rhs());
    for (const auto& [var, coeff] : t_ctr.row()) {
        if (var.is_virtual()) { continue; }
        set_coefficient(t_ctr, var, coeff);
    }
}

void Lpsolve::remove_variable(const Var &t_var) {
    int zero_i = 0;
    double zero_d = 0;

    auto success = set_columnex(model, get(t_var), 1, &zero_d, &zero_i);
    throw_if_error(success, "Could not remove variable");

    m_free_columns.push(get(t_var));
}

void Lpsolve::remove_constraint(const Ctr &t_ctr) {

    const int n_entries = 1 + (int) t_ctr.row().size();
    auto* colno = new int[n_entries];
    auto* row = new double[n_entries];

    int i = 0;
    colno[i] = 0;
    row[i] = 0.;
    ++i;

    for (const auto& [ctr, coefficient] : t_ctr.row()) {
        colno[i] = get(ctr);
        row[i] = 0.;
        ++i;
    }

    auto success = set_rowex(model, get(t_ctr), n_entries, row, colno);
    throw_if_error(success, "Could not remove constraint");

    delete[] colno;
    delete[] row;

    m_free_constraints.push(get(t_ctr));
}

void Lpsolve::set_objective_coefficient(const Var &t_var, const Coefficient &t_coeff) {
    const auto success = set_obj(model, get(t_var), value(t_coeff));
    throw_if_error(success, "could not set objective coefficient");
}

void Lpsolve::set_rhs(const Ctr &t_ctr, const Coefficient &t_coeff) {
    const auto success = set_rh(model, get(t_ctr), value(t_coeff));
    throw_if_error(success, "could not set RHS");
}

void Lpsolve::set_coefficient(const Ctr &t_ctr, const Var &t_var, const Coefficient &t_coefficient) {
    const auto success = set_mat(model, get(t_ctr), get(t_var), value(t_coefficient));
    throw_if_error(success, "could not set coefficient");
}

void Lpsolve::set_lb(const Var &t_var, double t_lb) {
    const auto success = set_lowbo(model, get(t_var), t_lb);
    throw_if_error(success, "could not set LB");
}

void Lpsolve::set_ub(const Var &t_var, double t_ub) {
    const auto success = set_upbo(model, get(t_var), t_ub);
    throw_if_error(success, "could not set UB");
}

void Lpsolve::set_type(const Var &t_var, VarType t_type) {
    
    unsigned char success;
    const VarType type = t_var.type();
    const auto var = get(t_var);
    
    if (type == Integer) {
        success = set_int(model, var, true);
    } else if (type == Binary) {
        success = set_binary(model, var, true);
    } else {
        success = set_int(model, var, false);
        success = success && set_binary(model, var, false);
    }
    
    throw_if_error(success, "could not update variable type");

}

void Lpsolve::set_type(const Ctr &t_ctr, CtrType t_type) {
    char type;
    switch (t_type) {
        case LessOrEqual: type = LE; break;
        case GreaterOrEqual: type = GE; break;
        case Equal: type = EQ; break;
        default: throw std::runtime_error("Unkown variable type");
    }
    const auto success = set_constr_type(model, get(t_ctr), type);
    throw_if_error(success, "could not set constraint type");
}

SolutionStatus Lpsolve::get_primal_status() const {
    if (!m_solution_status) {
        throw std::runtime_error("No solution found.");
    }
    return *m_solution_status;
}

double Lpsolve::get_primal_objective_value() const {
    return m_objective_offset + get_objective(model);
}

double Lpsolve::get_primal_value(const Var &t_var) const {
    return get_var_primalresult(model, get_Norig_rows(model) + get(t_var));
}

double Lpsolve::get_dual_value(const Ctr &t_ctr) const {
    return get_var_dualresult(model, get(t_ctr));
}

void Lpsolve::set_infeasible_or_unbounded_info(bool t_value) {
    m_infeasible_or_unbounded_info = t_value;
}

double Lpsolve::get_unbounded_ray(const Var &t_var) const {
    return get_primal_value(t_var);
}

double Lpsolve::get_unbounded_ray_objective_value() const {
    return get_primal_objective_value();
}

bool Lpsolve::infeasible_or_unbounded_info() const {
    return m_infeasible_or_unbounded_info;
}

void Lpsolve::compute_unbounded_ray() {

    std::vector<double> ones(get_Norig_columns(model)+1, 1.);
    add_constraint(model, ones.data(), LE, 1);

    for (const auto& ctr : source_model().constraints()) {
        set_rh(model, get(ctr), 0.);
    }

    const auto status = ::solve(model);

    m_ray = BaseSolver::unbounded_ray();

    del_constraint(model, get_Norig_rows(model));
    for (const auto& ctr : source_model().constraints()) {
        set_rh(model, get(ctr), value(ctr.rhs()));
    }

}

void Lpsolve::compute_farkas_dual() {

    const unsigned int n_original_variables = get_Norig_columns(model);
    unsigned int n_artificial_variables = 0;
    const double plus_one = 1.;
    const double minus_one = -1.;

    for (const auto& ctr : source_model().constraints()) {

        const int index = get(ctr);
        const auto type = ctr.type();

        if (type == LessOrEqual) {
            add_columnex(model, 1, (double*) &minus_one, (int*) &index);
            n_artificial_variables += 1;
        } else if (type == GreaterOrEqual) {
            add_columnex(model, 1, (double*) &plus_one, (int*) &index);
            n_artificial_variables += 1;
        } else {
            add_columnex(model, 1, (double*) &minus_one, (int*) &index);
            add_columnex(model, 1, (double*) &plus_one, (int*) &index);
            n_artificial_variables += 2;
        }

    }

    const unsigned int n_total_variables = n_original_variables + n_artificial_variables;

    std::vector<double> objective;
    objective.reserve(1 + n_total_variables);
    unsigned int i = 0;
    for ( ; i <= n_original_variables ; ++i) { objective.emplace_back(0.); }
    for ( ; i <= n_total_variables ; ++i) { objective.emplace_back(1.); }

    set_obj_fn(model, objective.data());

    ::solve(model);

    m_farkas = BaseSolver::dual_farkas();

    for ( ; i > n_original_variables ; --i) {
        del_column(model, i);
    }

    for (const auto& [var, coeff] : source_model().objective()) {
        set_objective_coefficient(var, coeff);
    }

}

double Lpsolve::get_dual_farkas_objective_value() const {
    double result = 0.;
    for (const auto& ctr : source_model().constraints()) {
        result += get_dual_farkas_value(ctr) * value(ctr.rhs());
    }
    return result;
}

double Lpsolve::get_dual_farkas_value(const Ctr &t_ctr) const {
    return get_dual_value(t_ctr);
}

Solution::Primal Lpsolve::unbounded_ray() const {
    return m_ray.value();
}

Solution::Dual Lpsolve::dual_farkas() const {
    return m_farkas.value();
}

void Lpsolve::set_algorithm_for_lp(AlgorithmForLP t_algorithm) {
    int algorithm;
    switch (t_algorithm) {
        case Automatic: algorithm = SIMPLEX_DUAL_PRIMAL; break;
        case PrimalSimplex: algorithm = SIMPLEX_PRIMAL_PRIMAL;  break;
        case DualSimplex: algorithm = SIMPLEX_DUAL_DUAL; break;
        case Barrier: throw std::runtime_error("lpsolve does not have a barrier solver.");
        default: throw std::runtime_error("Did not know what to do with algorithm " + std::to_string(t_algorithm));
    }
    set_simplextype(model, algorithm);
}

AlgorithmForLP Lpsolve::algorithm_for_lp() const {
    int algorithm = get_simplextype(model);
    if (algorithm == SIMPLEX_PRIMAL_PRIMAL || algorithm == SIMPLEX_DUAL_PRIMAL) { return PrimalSimplex; }
    if (algorithm == SIMPLEX_DUAL_DUAL || algorithm == SIMPLEX_PRIMAL_DUAL) { return DualSimplex; }
    throw std::runtime_error("Did not know what to do with algorithm " + std::to_string(algorithm));
}

void Lpsolve::set_presolve(bool t_value) {
    throw std::runtime_error("Not implemented.");
}

bool Lpsolve::presolve() const {
    throw std::runtime_error("Not implemented.");
}

void Lpsolve::set_objective_offset(const Coefficient &t_offset) {
    m_objective_offset = value(t_offset);
}


#endif