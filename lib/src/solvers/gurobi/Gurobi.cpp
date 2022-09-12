//
// Created by henri on 09/09/22.
//
#ifdef USE_GUROBI
#include "solvers/gurobi/Gurobi.h"

GRBEnv Gurobi::m_env = GRBEnv();

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
    m_model.set(GRB_IntParam_OutputFlag, 0);
    m_model.set(GRB_IntParam_InfUnbdInfo, true); std::cout << "inf or undb info = true" << std::endl;
    //m_model.set(GRB_IntParam_Presolve, false);
    init_model(t_model);
}

GRBVar Gurobi::create_variable(const Var &t_var) {
    return m_model.addVar(t_var.lb(), t_var.ub(), value(t_var.obj()), gurobi_type(t_var.type()), t_var.name());
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
    get(t_var).set(GRB_DoubleAttr_Obj, value(t_coeff));
}

void Gurobi::set_rhs(const Ctr &t_ctr, const Coefficient &t_coeff) {
    get(t_ctr).set(GRB_DoubleAttr_RHS, value(t_coeff));
}

void Gurobi::set_coefficient(const Ctr &t_ctr, const Var &t_var, const Coefficient &t_coefficient) {
    m_model.chgCoeff(get(t_ctr), get(t_var), value(t_coefficient));
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

SolutionStatus Gurobi::get_status() const {
    SolutionStatus status = Unknown;
    auto grb_status = m_model.get(GRB_IntAttr_Status);
    switch (grb_status) {
        case GRB_OPTIMAL: status = Optimal; break;
        case GRB_INFEASIBLE: status = Infeasible; break;
        case GRB_INF_OR_UNBD: status = InfeasibleOrUnbounded; break;
        case GRB_UNBOUNDED: status = Unbounded; break;
        case GRB_TIME_LIMIT: status = m_model.get(GRB_IntAttr_SolCount) > 0 ? FeasibleTimeLimit : InfeasibleTimeLimit; break;
        case GRB_NUMERIC: status = Error; break;
        default: throw std::runtime_error("Did not know what to do with gurobi status: " + std::to_string(grb_status));
    }
    return status;
}

double Gurobi::get_objective_value() const {
    return m_model.get(GRB_DoubleAttr_ObjVal);
}

double Gurobi::get_primal_value(const Var &t_var) const {
    if (get_status() == Unbounded) {
        return get(t_var).get(GRB_DoubleAttr_UnbdRay);
    }
    return get(t_var).get(GRB_DoubleAttr_X);
}

double Gurobi::get_dual_value(const Ctr &t_ctr) const {
    if (get_status() == Infeasible) {
        return get(t_ctr).get(GRB_DoubleAttr_FarkasDual);
    }
    return get(t_ctr).get(GRB_DoubleAttr_Pi);
}

double Gurobi::get_reduced_cost(const Var &t_var) const {
    return get(t_var).get(GRB_DoubleAttr_RC);
}

#endif
