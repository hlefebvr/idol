//
// Created by henri on 11/10/22.
//
#include "../../../include/algorithms/solvers/Solvers_Gurobi.h"

#ifdef IDOL_USE_GUROBI

GRBEnv Solvers::Gurobi::m_env = GRBEnv();

Solvers::Gurobi::Gurobi(Model &t_model) : Solver(t_model), m_model(m_env) {

    m_model.set(GRB_IntParam_OutputFlag, 0);

    set_callback_attribute<Attr::InfeasibleOrUnboundedInfo>([this](bool t_value) {
        m_model.set(GRB_IntParam_InfUnbdInfo, t_value);
    });

    set_callback_attribute<Attr::Presolve>([this](bool t_value) {
        m_model.set(GRB_IntParam_Presolve, t_value);
    });

    set_callback_attribute<Attr::MipGap>([this](double t_value){
        m_model.set(GRB_DoubleParam_MIPGap, t_value);
    });

    set_callback_attribute<Attr::CutOff>([this](double t_value){
        m_model.set(GRB_DoubleParam_Cutoff, t_value);
    });

    for (const auto& var : t_model.variables()) {
        auto impl = create_variable_impl_with_objective_coefficient(var);
        add_variable_impl(impl);
    }

    for (const auto& ctr : t_model.constraints()) {
        add_constraint_impl(GurobiCtr(ctr));
    }

}

void Solvers::Gurobi::update() {

    if(!m_is_built) {

        for (auto& gurobi_ctr : constraints()) {
            gurobi_ctr.update(*this);
        }

        m_is_built = true;
        return;
    }

    for (const auto& ctr : constraint_update_requests()) {
        if (ctr.status() == Removed) { continue; }
        raw(ctr).update(*this);
    }
    clear_constraint_update_requests();

}

void Solvers::Gurobi::execute() {

    update();

    m_model.optimize();

    if (m_model.get(GRB_IntAttr_Status) == GRB_INFEASIBLE && m_model.get(GRB_IntParam_InfUnbdInfo)) {
        m_model.computeIIS();
    }

    analyze_status();

}

void Solvers::Gurobi::analyze_status() {
    auto gurobi_status = m_model.get(GRB_IntAttr_Status);
    switch (gurobi_status) {
        case GRB_OPTIMAL:
            set_status(Optimal);
            set_reason(Proved);
        break;
        case GRB_INFEASIBLE:
            set_status(Infeasible);
            set_reason(Proved);
        break;
        case GRB_INF_OR_UNBD:
            set_status(InfeasibleOrUnbounded);
            set_reason(Proved);
        break;
        case GRB_UNBOUNDED:
            set_status(Unbounded);
            set_reason(Proved);
        break;
        case GRB_CUTOFF:
            set_status(Unknown);
            set_reason(CutOff);
        break;
        case GRB_TIME_LIMIT:
            set_status(m_model.get(GRB_IntAttr_SolCount) > 0 ? Feasible : Infeasible);
            set_reason(TimeLimit);
        break;
        case GRB_NUMERIC:
            set_status(Fail);
            set_reason(NotSpecified);
        break;
        default:
            throw Exception("Gurobi returned with status " + std::to_string(gurobi_status) + " which is not handled.");
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
    auto result =  m_model.addConstr(0., gurobi_type(t_ctr.type()), value(t_ctr.rhs()), t_ctr.name());
    return result;
}

void GurobiCtr::update(Solvers::Gurobi &t_parent) {

    if (has_impl()) { return; }

    GRBLinExpr expr;
    for (const auto& [var, constant] : m_constraint.row().lhs()) {
        expr += constant.numerical() * t_parent.raw(var);
    }
    m_impl = t_parent.m_model.addConstr(expr,
                               t_parent.gurobi_type(m_constraint.type()),
                               m_constraint.rhs().numerical());

}

void Solvers::Gurobi::set_constraint_lhs(const Ctr &t_ctr) {
    for (const auto& [var, coeff] : t_ctr.row().lhs()) {
        m_model.chgCoeff(raw(t_ctr).impl(), raw(var), value(coeff));
    }
}

void Solvers::Gurobi::set_variable_components(const Var &t_var) {
    for (const auto& [ctr, coeff] : t_var.column().components()) {
        m_model.chgCoeff(raw(ctr).impl(), raw(t_var), value(coeff));
    }
}

void Solvers::Gurobi::update_coefficient_rhs(const Ctr &t_ctr, double t_rhs) {
    raw(t_ctr).impl().set(GRB_DoubleAttr_RHS, t_rhs);
    model().update_rhs(t_ctr, t_rhs);
}

void Solvers::Gurobi::remove(const Var &t_variable) {
    m_model.remove(raw(t_variable));

    remove_variable_impl(t_variable);
    model().remove(t_variable);
}

Solution::Dual Solvers::Gurobi::farkas_certificate() const {

    if (status() != Infeasible) {
        throw Exception("Only available for infeasible problems.");
    }

    if (!get<Attr::InfeasibleOrUnboundedInfo>()) {
        throw Exception("Turn on InfeasibleOrUnboundedInfo before solving your model to access farkas dual information.");
    }

    Solution::Dual result;

    result.set_objective_value(m_model.get(GRB_DoubleAttr_FarkasProof));

    for (const auto& ctr : model().constraints()) {
        result.set(ctr, -raw(ctr).impl().get(GRB_DoubleAttr_FarkasDual));
    }

    return result;
}

Solution::Primal Solvers::Gurobi::unbounded_ray() const {

    if (status() != Unbounded) {
        throw Exception("Only available for unbounded problems.");
    }

    if (!get<Attr::InfeasibleOrUnboundedInfo>()) {
        throw Exception("Turn on InfeasibleOrUnboundedInfo before solving your model to access extreme ray information.");
    }

    Solution::Primal result;

    double objective_value = 0.;
    for (const auto& [var, coeff] : model().objective()) {
        objective_value += raw(var).get(GRB_DoubleAttr_UnbdRay) * value(coeff);
    }

    result.set_objective_value(objective_value);

    for (const auto& var : model().variables()) {
        result.set(var, raw(var).get(GRB_DoubleAttr_UnbdRay));
    }

    return result;
}

Solution::Primal Solvers::Gurobi::primal_solution(
             SolutionStatus t_status,
             Reason t_reason,
             const std::function<double()>& t_get_obj_val,
             const std::function<double(const GRBVar&)>& t_get_primal_value) const {
    Solution::Primal result;
    result.set_status(t_status);
    result.set_reason(t_reason);

    if (t_status == Unbounded) {
        result.set_objective_value(-Inf);
        return result;
    }

    if (t_status == Infeasible) {
        result.set_objective_value(+Inf);
        return result;
    }

    if (!is_in(t_status, { Optimal, Feasible })) {
        result.set_objective_value(0.);
        return result;
    }

    result.set_objective_value(t_get_obj_val());

    for (const auto& var : model().variables()) {
        result.set(var, t_get_primal_value(raw(var)));
    }

    for (auto& cb : m_callbacks) {
        result.merge_without_conflict(cb->help());
    }

    return result;
}

Solution::Primal Solvers::Gurobi::primal_solution() const {
    /* if (m_current_solution != 0) {
        return primal_solution(
                solution_status(),
                [this](){ return m_model.get(GRB_DoubleAttr_PoolObjVal); },
                [this](const GRBVar& t_var){ return t_var.get(GRB_DoubleAttr_Xn); }
        );
    } */
    return primal_solution(
            status(),
            reason(),
            [this](){ return m_model.get(GRB_DoubleAttr_ObjVal); },
            [](const GRBVar& t_var){ return t_var.get(GRB_DoubleAttr_X); }
        );
}

Solution::Dual Solvers::Gurobi::dual_solution(SolutionStatus t_status, const std::function<double()> &t_get_obj_val,
                                              const std::function<double(const GRBConstr &)> &t_get_dual_value) const {
    Solution::Dual result;
    const auto dual_status = t_status;
    result.set_status(dual_status);

    if (dual_status == Unbounded) {
        result.set_objective_value(+Inf);
        return result;
    }

    if (is_in(dual_status, { Infeasible })) {
        result.set_objective_value(-Inf);
        return result;
    }

    if (!is_in(result.status(), { Optimal, Feasible })) {
        result.set_objective_value(0.);
        return result;
    }

    result.set_objective_value(t_get_obj_val());

    if (!is_in(result.status(), { Optimal, Feasible })) {
        return result;
    }

    for (const auto& ctr : model().constraints()) {
        result.set(ctr, t_get_dual_value(raw(ctr).impl()));
    }

    return result;
}

Solution::Dual Solvers::Gurobi::dual_solution() const {
    return dual_solution(
            dual(status()),
            [this](){ return m_model.get(GRB_DoubleAttr_ObjVal); },
            [this](const GRBConstr& t_ctr){ return t_ctr.get(GRB_DoubleAttr_Pi); }
            );
}

Solution::Dual Solvers::Gurobi::iis() const {
    return dual_solution(
            Optimal,
            [](){ return Inf; },
            [this](const GRBConstr& t_ctr){ return t_ctr.get(GRB_IntAttr_IISConstr); }
    );
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
    add_constraint_impl(GurobiCtr(ctr));
    add_constraint_update_request(ctr);
    return ctr;
}

void Solvers::Gurobi::remove(const Ctr &t_constraint) {
    auto& gurobi_ctr = raw(t_constraint);
    if (gurobi_ctr.has_impl()) {
        m_model.remove(raw(t_constraint).impl());
    }
    remove_constraint_impl(t_constraint);
    model().remove(t_constraint);
}

void Solvers::Gurobi::update_lb(const Var &t_var, double t_lb) {
    raw(t_var).set(GRB_DoubleAttr_LB, t_lb);
    model().update_lb(t_var, t_lb);

    for (auto& ptr_to_cb : m_callbacks) {
        ptr_to_cb->update_lb(t_var, t_lb);
    }
}

void Solvers::Gurobi::update_ub(const Var &t_var, double t_ub) {
    raw(t_var).set(GRB_DoubleAttr_UB, t_ub);
    model().update_ub(t_var, t_ub);

    for (auto& ptr_to_cb : m_callbacks) {
        ptr_to_cb->update_ub(t_var, t_ub);
    }
}

void Solvers::Gurobi::write(const std::string &t_filename) {
    m_model.write(t_filename + ".lp");
}

void Solvers::Gurobi::execute_iis() {
    m_model.computeIIS();
}

#endif
