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

    init_model(t_model);
}

Lpsolve::~Lpsolve() {
    free_lp(&model);
}

void Lpsolve::write(const std::string &t_filename) {
    write_lp(model, (char*) t_filename.c_str());
}

void Lpsolve::solve() {
    int status = ::solve(model);

    switch (status) {
        case NUMFAILURE: [[fallthrough]];
        case NOMEMORY: m_solution_status = Error; break;
        case PRESOLVED: [[fallthrough]];
        case OPTIMAL: m_solution_status = Optimal; break;
        case SUBOPTIMAL: m_solution_status = FeasibleTimeLimit; break;
        case INFEASIBLE: m_solution_status = Infeasible; break;
        case UNBOUNDED: m_solution_status = Unbounded; break;
        case TIMEOUT: m_solution_status = InfeasibleTimeLimit; break;
        default: throw std::runtime_error("Did not know what to do with lpsolve status: " + std::to_string(status));
    }

}

int Lpsolve::create_variable(const Var &t_var) {
    
    const auto index = (int) t_var.index() + 1;
    unsigned char success;
    
    double coeff = value(t_var.obj());
    int row_id = 0;
    
    success = add_columnex(model, 1, &coeff, &row_id);
    throw_if_error(success, "could not add column");
    
    success = set_bounds(model, index, t_var.lb(), t_var.ub());
    throw_if_error(success, "could not set bounds");
    
    switch (t_var.type()) {
        case Integer: success = set_int(model, index, true); break;
        case Binary: success = set_binary(model, index, true); break;
        default: break;
    }
    throw_if_error(success, "could not set variable type");
    
    success = set_col_name(model, index, (char*) t_var.name().c_str());
    throw_if_error(success, "could not set variable name");

    return index;
}

int Lpsolve::create_constraint(const Ctr &t_ctr) {
    
    const auto index = (int) t_ctr.index() + 1;
    unsigned char success;
    
    const double coeff = value(t_ctr.rhs());
    
    success = set_add_rowmode(model, true);
    throw_if_error(success, "could not enter rowmode");

    success = add_constraintex(model, 0, NULL, NULL, LE, coeff);
    throw_if_error(success, "could not add constraint");

    success = set_add_rowmode(model, false);
    throw_if_error(success, "could not exit rowmode");

    success = set_row_name(model, index, (char*) t_ctr.name().c_str());
    throw_if_error(success, "could not set constraint name");
    
    return index;
}

void Lpsolve::fill_column(const Var &t_var) {
    set_objective_coefficient(t_var, t_var.obj());
    for (const auto& [ctr, coeff] : t_var.column()) {
        set_coefficient(ctr, t_var, coeff);
    }
}

void Lpsolve::fill_row(const Ctr &t_ctr) {
    set_rhs(t_ctr, t_ctr.rhs());
    for (const auto& [var, coeff] : t_ctr.row()) {
        set_coefficient(t_ctr, var, coeff);
    }
}

void Lpsolve::remove_variable(const Var &t_var) {
    throw std::runtime_error("Not implemented.");
}

void Lpsolve::remove_constraint(const Ctr &t_ctr) {
    throw std::runtime_error("Not implemented.");
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

SolutionStatus Lpsolve::get_status() const {
    if (!m_solution_status) {
        throw std::runtime_error("No solution found.");
    }
    return *m_solution_status;
}

double Lpsolve::get_objective_value() const {
    return get_objective(model);
}

double Lpsolve::get_primal_value(const Var &t_var) const {
    return get_var_primalresult(model, get_Norig_rows(model) + get(t_var));
}

double Lpsolve::get_dual_value(const Ctr &t_ctr) const {
    return get_var_dualresult(model, get(t_ctr));
}

double Lpsolve::get_reduced_cost(const Var &t_var) const {
    return get_var_dualresult(model, get_Norig_rows(model) + get(t_var));
}

#endif