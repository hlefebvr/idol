//
// Created by henri on 09/09/22.
//
#ifdef USE_GUROBI
#include "solvers/gurobi/Gurobi.h"


char Gurobi::gurobi_type(CtrType t_type) {
    if (t_type == Equal) {
        return GRB_EQUAL;
    }
    if (t_type == LessOrEqual) {
        return GRB_LESS_EQUAL;
    }
    return GRB_GREATER_EQUAL;
}

char Gurobi::gurobi_type(VarType t_type) {
    if (t_type == Continuous) {
        return GRB_CONTINUOUS;
    }
    if (t_type == Integer) {
        return GRB_INTEGER;
    }
    return GRB_BINARY;
}

Gurobi::Gurobi(Model &t_model) : BaseSolver<GRBVar, GRBConstr>(t_model), m_model(m_env) {
    init_model(t_model);
}

GRBVar Gurobi::create_variable(const Var &t_var) {
    return m_model.addVar(t_var.lb(), t_var.ub(), t_var.obj().constant(), gurobi_type(t_var.type()), t_var.name());
}

GRBConstr Gurobi::create_constraint(const Ctr &t_ctr) {
    return m_model.addConstr(0., gurobi_type(t_ctr.type()), 0., t_ctr.name());
}

void Gurobi::fill_column(const Var &t_var) {
    set_objective_coefficient(t_var, t_var.obj());
    for (const auto& [ctr, coeff] : t_var.column()) {
        set_coefficient(ctr, t_var, coeff);
    }
}

void Gurobi::fill_row(const Ctr &t_ctr) {
    set_rhs(t_ctr, t_ctr.rhs());
    for (const auto& [var, coeff] : t_ctr.row()) {
        set_coefficient(t_ctr, var, coeff);
    }
}

void Gurobi::remove_variable(const Var &t_var) {
    m_model.remove(get(t_var));
}

void Gurobi::remove_constraint(const Ctr &t_ctr) {
    m_model.remove(get(t_ctr));
}

void Gurobi::set_objective_coefficient(const Var &t_var, const Coefficient &t_coeff) {
    get(t_var).set(GRB_DoubleAttr_Obj, t_coeff.constant());
}

void Gurobi::set_rhs(const Ctr &t_ctr, const Coefficient &t_coeff) {
    get(t_ctr).set(GRB_DoubleAttr_RHS, t_coeff.constant());
}

void Gurobi::set_coefficient(const Ctr &t_ctr, const Var &t_var, const Coefficient &t_coefficient) {
    m_model.chgCoeff(get(t_ctr), get(t_var), t_coefficient.constant());
}

void Gurobi::set_lb(const Var &t_var, double t_lb) {
    get(t_var).set(GRB_DoubleAttr_LB, t_lb);
}

void Gurobi::set_ub(const Var &t_var, double t_ub) {
    get(t_var).set(GRB_DoubleAttr_LB, t_ub);
}

void Gurobi::set_type(const Var &t_var, VarType t_type) {
    get(t_var).set(GRB_CharAttr_VType, gurobi_type(t_var.type()));
}

void Gurobi::set_type(const Ctr &t_ctr, CtrType t_type) {
    get(t_ctr).set(GRB_CharAttr_Sense, gurobi_type(t_ctr.type()));
}

void Gurobi::write(const std::string &t_filename) {
    m_model.write(t_filename);
}

void Gurobi::solve() {
    m_model.optimize();
}

#endif
