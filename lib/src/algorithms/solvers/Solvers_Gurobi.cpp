//
// Created by henri on 11/10/22.
//
#include "../../../include/algorithms/solvers/Solvers_Gurobi.h"

#ifdef IDOL_USE_GUROBI

GRBEnv Solvers::Gurobi::m_env = GRBEnv();

Solvers::Gurobi::Gurobi(Model &t_model) : Solver(t_model), m_model(m_env) {

    m_model.set(GRB_IntParam_OutputFlag, 0);

    m_attributes.template set_callback<Attr::InfeasibleOrUnboundedInfo>([this](bool t_value) {
        m_model.set(GRB_IntParam_InfUnbdInfo, t_value);
    });

    m_attributes.template set_callback<Attr::Presolve>([this](bool t_value) {
        m_model.set(GRB_IntParam_Presolve, t_value);
    });

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

void Solvers::Gurobi::execute() {

    m_model.optimize();

    if (m_model.get(GRB_IntAttr_Status) == GRB_INFEASIBLE && m_model.get(GRB_IntParam_InfUnbdInfo)) {
        m_model.computeIIS();
    }

}

char Solvers::Gurobi::gurobi_type(CtrType t_type) {
    if (t_type == Equal) {
        return GRB_EQUAL;
    }
    if (t_type == LessOrEqual) {
        return GRB_LESS_EQUAL;
    }
    return GRB_GREATER_EQUAL;
}

char Solvers::Gurobi::gurobi_type(VarType t_type) {
    if (t_type == Continuous) {
        return GRB_CONTINUOUS;
    }
    if (t_type == Integer) {
        return GRB_INTEGER;
    }
    return GRB_BINARY;
}
GRBVar Solvers::Gurobi::create_variable_impl_with_objective_coefficient(const Var &t_var) {
    return m_model.addVar(t_var.lb(), t_var.ub(), value(t_var.obj()), gurobi_type(t_var.type()), t_var.name());
}

GRBConstr Solvers::Gurobi::create_constraint_impl_with_rhs(const Ctr &t_ctr) {
    return m_model.addConstr(0., gurobi_type(t_ctr.type()), value(t_ctr.rhs()), t_ctr.name());
}

void Solvers::Gurobi::set_constraint_lhs(const Ctr &t_ctr) {
    for (const auto& [var, coeff] : t_ctr.row().lhs()) {
        m_model.chgCoeff(raw(t_ctr), raw(var), value(coeff));
    }
}

void Solvers::Gurobi::set_variable_components(const Var &t_var) {
    for (const auto& [ctr, coeff] : t_var.column().components()) {
        m_model.chgCoeff(raw(ctr), raw(t_var), value(coeff));
    }
}

SolutionStatus Solvers::Gurobi::solution_status() const {
    SolutionStatus status = Unknown;
    auto grb_status = m_model.get(GRB_IntAttr_Status);
    switch (grb_status) {
        case GRB_OPTIMAL: status = Optimal; break;
        case GRB_INFEASIBLE: status = Infeasible; break;
        case GRB_INF_OR_UNBD: status = InfeasibleOrUnbounded; break;
        case GRB_UNBOUNDED: status = Unbounded; break;
        case GRB_TIME_LIMIT: status = m_model.get(GRB_IntAttr_SolCount) > 0 ? FeasibleTimeLimit : InfeasibleTimeLimit; break;
        case GRB_NUMERIC: status = Fail; break;
        default: throw Exception("Did not know what to do with gurobi status: " + std::to_string(grb_status));
    }
    return status;
}

void Solvers::Gurobi::update_coefficient_rhs(const Ctr &t_ctr, double t_rhs) {
    raw(t_ctr).set(GRB_DoubleAttr_RHS, t_rhs);
    model().update_rhs(t_ctr, t_rhs);
}

void Solvers::Gurobi::remove(const Var &t_variable) {
    m_model.remove(raw(t_variable));

    remove_variable_impl(t_variable);
    model().remove(t_variable);
}

Solution::Dual Solvers::Gurobi::farkas_certificate() const {

    if (solution_status() != Infeasible) {
        throw Exception("Only available for infeasible problems.");
    }

    if (!Algorithm::get<Attr::InfeasibleOrUnboundedInfo>()) {
        throw Exception("Turn on infeasible_or_unbounded_info before solving your model to access farkas dual information.");
    }

    Solution::Dual result;

    result.set_objective_value(m_model.get(GRB_DoubleAttr_FarkasProof));

    for (const auto& ctr : model().constraints()) {
        try {
            result.set(ctr, -raw(ctr).get(GRB_DoubleAttr_FarkasDual));
        } catch (const GRBException& t_err) {
            std::cout << t_err.getMessage() << std::endl;
            __throw_exception_again;
        }
    }

    return result;
}

Solution::Primal Solvers::Gurobi::primal_solution() const {
    Solution::Primal result;
    const auto status = solution_status();
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

    result.set_objective_value(m_model.get(GRB_DoubleAttr_ObjVal));

    for (const auto& var : model().variables()) {
        result.set(var, raw(var).get(GRB_DoubleAttr_X));
    }

    return result;
}

Solution::Dual Solvers::Gurobi::dual_solution() const {
    Solution::Dual result;
    const auto dual_status = dual(solution_status());
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

    result.set_objective_value(m_model.get(GRB_DoubleAttr_ObjVal));

    if (!is_in(result.status(), { Optimal, Feasible, FeasibleTimeLimit })) {
        return result;
    }

    for (const auto& ctr : model().constraints()) {
        result.set(ctr, raw(ctr).get(GRB_DoubleAttr_Pi));
    }

    return result;
}

void Solvers::Gurobi::update_objective(const Row &t_objective) {
    m_model.set(GRB_DoubleAttr_ObjCon, value(value(t_objective.rhs())));
    // TODO this must be better done after refacto of Objective and Rhs
    for (const auto& var : model().variables()) {
        raw(var).set(GRB_DoubleAttr_Obj, value(t_objective.lhs().get(var)));
    }
    model().update_objective(t_objective);
}

Var Solvers::Gurobi::add_column(TempVar t_temporary_variable) {
    auto var = model().add_variable(std::move(t_temporary_variable));
    auto impl = create_variable_impl_with_objective_coefficient(var);
    add_variable_impl(impl);
    set_variable_components(var);
    return var;
}

Ctr Solvers::Gurobi::add_row(TempCtr t_temporary_constraint) {
    auto ctr = model().add_constraint(std::move(t_temporary_constraint));
    auto impl = create_constraint_impl_with_rhs(ctr);
    add_constraint_impl(impl);
    set_constraint_lhs(ctr);
    return ctr;
}

void Solvers::Gurobi::remove(const Ctr &t_constraint) {
    m_model.remove(raw(t_constraint));
    remove_constraint_impl(t_constraint);
    model().remove(t_constraint);
}

void Solvers::Gurobi::update_lb(const Var &t_var, double t_lb) {
    raw(t_var).set(GRB_DoubleAttr_LB, t_lb);
    model().update_lb(t_var, t_lb);
}

void Solvers::Gurobi::update_ub(const Var &t_var, double t_ub) {
    raw(t_var).set(GRB_DoubleAttr_UB, t_ub);
    model().update_ub(t_var, t_ub);
}

#endif