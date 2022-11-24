//
// Created by henri on 11/10/22.
//
#include "../../../include/algorithms/solvers/Solvers_Gurobi.h"

#ifdef IDOL_USE_GUROBI

GRBEnv Solvers::Gurobi::m_env = GRBEnv();

Solvers::Gurobi::Gurobi(Model &t_model) : Solver(t_model), m_model(m_env) {

    m_model.set(GRB_IntParam_OutputFlag, 0);

    m_model.set(GRB_IntAttr_ModelSense, t_model.sense() == Minimize ? GRB_MINIMIZE : GRB_MAXIMIZE);

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

    set_callback_attribute<Attr::BestObjStop>([this](double t_value){
        m_model.set(GRB_DoubleParam_BestObjStop, t_value);
    });

    set_callback_attribute<Attr::BestBoundStop>([this](double t_value){
        m_model.set(GRB_DoubleParam_BestBdStop, t_value);
    });

    set_callback_attribute<Attr::MaxThreads>([this](unsigned int t_num_threads) {
        m_model.set(GRB_IntParam_Threads, (int) t_num_threads);
    });

    for (const auto& var : t_model.vars()) {
        add_future(var, false);
    }

    for (const auto& ctr : t_model.ctrs()) {
        add_future(ctr);
    }

}

void Solvers::Gurobi::execute() {

    update();

    if (get<Attr::ResetBeforeSolving>()) {
        m_model.reset();
    }

    m_model.set(GRB_DoubleParam_TimeLimit, get<Attr::TimeLimit>());

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
        case GRB_USER_OBJ_LIMIT:
            set_status(Unknown);
            set_reason(UserObjLimit);
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

void Solvers::Gurobi::update_rhs_coeff(const Ctr &t_ctr, double t_rhs) {
    future(t_ctr).impl().set(GRB_DoubleAttr_RHS, t_rhs);
    model().update_rhs_coeff(t_ctr, t_rhs);
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

    for (const auto& ctr : model().ctrs()) {
        result.set(ctr, -future(ctr).impl().get(GRB_DoubleAttr_FarkasDual));
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
    for (const auto& [var, coeff] : model().obj().linear()) {
        objective_value += future(var).impl().get(GRB_DoubleAttr_UnbdRay) * value(coeff);
    }

    result.set_objective_value(objective_value);

    for (const auto& var : model().vars()) {
        result.set(var, future(var).impl().get(GRB_DoubleAttr_UnbdRay));
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

    for (const auto& var : model().vars()) {
        result.set(var, t_get_primal_value(future(var).impl()));
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

    for (const auto& ctr : model().ctrs()) {
        result.set(ctr, t_get_dual_value(future(ctr).impl()));
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

void Solvers::Gurobi::update_var_lb(const Var &t_var, double t_lb) {

    if (model().has(t_var)) {
        future(t_var).impl().set(GRB_DoubleAttr_LB, t_lb);
        model().update_var_lb(t_var, t_lb);
        return;
    }

    for (auto& ptr_to_cb : m_callbacks) {
        ptr_to_cb->update_var_lb(t_var, t_lb);
    }
}

void Solvers::Gurobi::update_var_ub(const Var &t_var, double t_ub) {

    if (model().has(t_var)) {
        future(t_var).impl().set(GRB_DoubleAttr_UB, t_ub);
        model().update_var_ub(t_var, t_ub);
        return;
    }

    for (auto& ptr_to_cb : m_callbacks) {
        ptr_to_cb->update_var_ub(t_var, t_ub);
    }
}

void Solvers::Gurobi::write(const std::string &t_filename) {
    update();
    m_model.write(t_filename + ".lp");
}

void Solvers::Gurobi::execute_iis() {
    update();
    m_model.set(GRB_DoubleParam_Cutoff, GRB_INFINITY);
    m_model.computeIIS();
}

GRBConstr Solvers::Gurobi::create(const Ctr &t_ctr, bool t_with_collaterals) {

    GRBLinExpr expr = 0.;
    if (t_with_collaterals) {
        for (const auto &[var, constant]: model().get_row(t_ctr).linear()) {
            expr += value(constant) * future(var).impl();
        }
    }

    return m_model.addConstr(expr, gurobi_type(model().get_type(t_ctr)), value(model().get_row(t_ctr).rhs()), t_ctr.name());

}

GRBVar Solvers::Gurobi::create(const Var &t_var, bool t_with_collaterals) {

    GRBColumn column;
    if (t_with_collaterals) {
        for (const auto& [ctr, constant] : get_column(t_var).linear()) {
            column.addTerm( value(constant), future(ctr).impl() );
        }
    }

    return m_model.addVar(get_lb(t_var), get_ub(t_var), value(get_column(t_var).obj()), gurobi_type(get_type(t_var)), column, t_var.name());
}

void Solvers::Gurobi::remove(const Var &t_var, GRBVar &t_impl) {
    m_model.remove(t_impl);
}

void Solvers::Gurobi::remove(const Ctr &t_ctr, GRBConstr &t_impl) {
    m_model.remove(t_impl);
}

void Solvers::Gurobi::update(const Var &t_var, GRBVar &t_impl) {
    std::cout << "SKIPPED UPDATE VAR" << std::endl;
}

void Solvers::Gurobi::update(const Ctr &t_ctr, GRBConstr &t_impl) {
    std::cout << "SKIPPED UPDATE CTR" << std::endl;
}

void Solvers::Gurobi::update_obj() {
    GRBLinExpr expr = value(model().obj().constant());
    for (const auto& [var, constant] : model().obj().linear()) {
        expr += value(constant) * future(var).impl();
    }
    m_model.setObjective(expr, model().sense() == Minimize ? GRB_MINIMIZE : GRB_MAXIMIZE);
}

#endif
