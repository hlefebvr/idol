//
// Created by henri on 11/10/22.
//
#include "../../../include/algorithms/solvers/Solvers_Gurobi.h"

#ifdef IDOL_USE_GUROBI

GRBEnv Solvers::Gurobi::m_env = GRBEnv();

Solvers::Gurobi::Gurobi(Model &t_model) : Solver(t_model), m_model(m_env) {

    m_model.set(GRB_IntParam_OutputFlag, 0);

    if (t_model.get(Attr::Obj::Expr).quadratic().empty()) {
        m_model.set(GRB_IntAttr_ModelSense, t_model.get(Attr::Obj::Sense) == Minimize ? GRB_MINIMIZE : GRB_MAXIMIZE);
    } else {
        add_future_obj();
    }

    for (const auto& var : t_model.vars()) {
        add_future(var, false);
    }

    for (const auto& ctr : t_model.ctrs()) {
        add_future(ctr);
    }

}

bool Solvers::Gurobi::set_parameter_double(const Parameter<double> &t_param, double t_value) {
    if (t_param.is_in_section(Param::Sections::Algorithm)) {

        if (t_param == Param::Algorithm::MIPGap) {
            m_model.set(GRB_DoubleParam_MIPGap, t_value);
        } else if (t_param == Param::Algorithm::CutOff) {
            m_model.set(GRB_DoubleParam_Cutoff, t_value);
        } else if (t_param == Param::Algorithm::BestObjStop) {
            m_model.set(GRB_DoubleParam_BestObjStop, t_value);
        } else if (t_param == Param::Algorithm::BestBoundStop) {
            m_model.set(GRB_DoubleParam_BestBdStop, t_value);
        }

        return Algorithm::set_parameter_double(t_param, t_value);
    }
    return false;
}

bool Solvers::Gurobi::set_parameter_int(const Parameter<int> &t_param, int t_value) {
    if (t_param.is_in_section(Param::Sections::Algorithm)) {

        if (t_param == Param::Algorithm::MaxThreads) {
            m_model.set(GRB_IntParam_Threads, t_value);
        }

        return Algorithm::set_parameter_int(t_param, t_value);
    }
    return false;
}

bool Solvers::Gurobi::set_parameter_bool(const Parameter<bool> &t_param, bool t_value) {
    if (t_param.is_in_section(Param::Sections::Algorithm)) {

        if (t_param == Param::Algorithm::InfeasibleOrUnboundedInfo) {
            m_model.set(GRB_IntParam_InfUnbdInfo, t_value);
        } else if (t_param == Param::Algorithm::Presolve) {
            m_model.set(GRB_IntParam_Presolve, t_value);
        }

        return Algorithm::set_parameter_bool(t_param, t_value);
    }
    return false;
}

void Solvers::Gurobi::execute() {

    update();

    if (get(Param::Algorithm::ResetBeforeSolving)) {
        m_model.reset();
    }

    m_model.set(GRB_DoubleParam_TimeLimit, get(Param::Algorithm::TimeLimit));

    m_model.optimize();

    if (m_model.get(GRB_IntAttr_Status) == GRB_INFEASIBLE && m_model.get(GRB_IntParam_InfUnbdInfo)) {
        m_model.computeIIS();
    }

    analyze_status();

    write("model.lp");

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

char Solvers::Gurobi::gurobi_ctr_type(int t_type) {
    if (t_type == Equal) {
        return GRB_EQUAL;
    }
    if (t_type == LessOrEqual) {
        return GRB_LESS_EQUAL;
    }
    return GRB_GREATER_EQUAL;
}

char Solvers::Gurobi::gurobi_var_type(int t_type) {
    if (t_type == Continuous) {
        return GRB_CONTINUOUS;
    }
    if (t_type == Integer) {
        return GRB_INTEGER;
    }
    return GRB_BINARY;
}

void Solvers::Gurobi::update_rhs_coeff(const Ctr &t_ctr, double t_rhs) {
    auto& impl = future(t_ctr).impl();

    if (std::holds_alternative<GRBConstr>(impl)) {
        std::get<GRBConstr>(impl).set(GRB_DoubleAttr_RHS, t_rhs);
    } else {
        std::get<GRBQConstr>(impl).set(GRB_DoubleAttr_QCRHS, t_rhs);
    }

    model().set(Attr::Ctr::Rhs, t_ctr, t_rhs);
}

Solution::Dual Solvers::Gurobi::farkas_certificate() const {

    if (status() != Infeasible) {
        throw Exception("Only available for infeasible problems.");
    }

    if (!get(Param::Algorithm::InfeasibleOrUnboundedInfo)) {
        throw Exception("Turn on InfeasibleOrUnboundedInfo before solving your model to access farkas dual information.");
    }

    Solution::Dual result;

    result.set_objective_value(m_model.get(GRB_DoubleAttr_FarkasProof));

    for (const auto& ctr : model().ctrs()) {
        auto& impl = future(ctr).impl();
        if (std::holds_alternative<GRBConstr>(impl)) {
            result.set(ctr, -std::get<GRBConstr>(impl).get(GRB_DoubleAttr_FarkasDual));
        } else {
            throw Exception("Farkas certificate not available for QCQPs.");
        }
    }

    return result;
}

Solution::Primal Solvers::Gurobi::unbounded_ray() const {

    if (status() != Unbounded) {
        throw Exception("Only available for unbounded problems.");
    }

    if (!get(Param::Algorithm::InfeasibleOrUnboundedInfo)) {
        throw Exception("Turn on InfeasibleOrUnboundedInfo before solving your model to access extreme ray information.");
    }

    Solution::Primal result;

    double objective_value = 0.;
    for (const auto& [var, coeff] : model().get(Attr::Obj::Expr).linear()) {
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
                                              const std::function<double(const std::variant<GRBConstr, GRBQConstr> &)> &t_get_dual_value) const {
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
        auto& impl = future(ctr).impl();
        result.set(ctr, t_get_dual_value(future(ctr).impl()));
    }

    return result;
}

Solution::Dual Solvers::Gurobi::dual_solution() const {
    return dual_solution(
            dual(status()),
            [this](){ return m_model.get(GRB_DoubleAttr_ObjVal); },
            [](const std::variant<GRBConstr, GRBQConstr>& t_ctr){

                if (std::holds_alternative<GRBConstr>(t_ctr)) {
                    return std::get<GRBConstr>(t_ctr).get(GRB_DoubleAttr_Pi);
                }
                return std::get<GRBQConstr>(t_ctr).get(GRB_DoubleAttr_QCPi);

            }
            );
}

Solution::Dual Solvers::Gurobi::iis() const {
    return dual_solution(
            Optimal,
            [](){ return Inf; },
            [](const std::variant<GRBConstr, GRBQConstr>& t_ctr){

                if (std::holds_alternative<GRBConstr>(t_ctr)) {
                    return std::get<GRBConstr>(t_ctr).get(GRB_IntAttr_IISConstr);
                }
                return std::get<GRBQConstr>(t_ctr).get(GRB_IntAttr_IISQConstr);

            }
    );
}

void Solvers::Gurobi::update_var_lb(const Var &t_var, double t_lb) {

    if (model().get(Attr::Var::Status, t_var)) {
        future(t_var).impl().set(GRB_DoubleAttr_LB, t_lb);
        model().set(Attr::Var::Lb, t_var, t_lb);
        return;
    }

    for (auto& ptr_to_cb : m_callbacks) {
        ptr_to_cb->update_var_lb(t_var, t_lb);
    }
}

void Solvers::Gurobi::update_var_ub(const Var &t_var, double t_ub) {

    if (model().get(Attr::Var::Status, t_var)) {
        future(t_var).impl().set(GRB_DoubleAttr_UB, t_ub);
        model().set(Attr::Var::Ub, t_var, t_ub);
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

std::variant<GRBConstr, GRBQConstr> Solvers::Gurobi::create(const Ctr &t_ctr, bool t_with_collaterals) {

    const auto& row = model().get(Attr::Ctr::Row, t_ctr);
    const auto type = gurobi_ctr_type(model().get(Attr::Ctr::Type, t_ctr));

    GRBLinExpr expr = 0.;
    if (t_with_collaterals) {

        for (const auto &[var, constant]: row.linear()) {
            expr += value(constant) * future(var).impl();
        }

    }

    return m_model.addConstr(expr, type, value(row.rhs()), t_ctr.name());

}

GRBVar Solvers::Gurobi::create(const Var &t_var, bool t_with_collaterals) {

    const auto& column = get_column(t_var);
    const auto lb = get_lb(t_var);
    const auto ub = get_ub(t_var);
    const auto objective = value(column.obj());
    const auto type = gurobi_var_type(get_type(t_var));
    const auto& name = t_var.name();

    GRBColumn col;
    if (t_with_collaterals) {

        for (const auto& [ctr, constant] : column.linear()) {

            auto& impl = future(ctr).impl();

            if (std::holds_alternative<GRBQConstr>(impl)) {
                throw Exception("Cannot add column to quadratic constraints.");
            }

            col.addTerm( value(constant), std::get<GRBConstr>(impl) );

        }

        if (!column.quadratic().empty()) {
            throw Exception("Cannot add column with quadratic terms.");
        }
    }

    return m_model.addVar(lb, ub, objective, type, col, name);
}

void Solvers::Gurobi::remove(const Var &t_var, GRBVar &t_impl) {
    m_model.remove(t_impl);
}

void Solvers::Gurobi::remove(const Ctr &t_ctr, std::variant<GRBConstr, GRBQConstr> &t_impl) {
    if (std::holds_alternative<GRBConstr>(t_impl)) {
        m_model.remove(std::get<GRBConstr>(t_impl));
    } else {
        m_model.remove(std::get<GRBQConstr>(t_impl));
    }
}

void Solvers::Gurobi::update(const Var &t_var, GRBVar &t_impl) {
    std::cout << "SKIPPED UPDATE VAR" << std::endl;
}

void Solvers::Gurobi::update(const Ctr &t_ctr, std::variant<GRBConstr, GRBQConstr> &t_impl) {
    std::cout << "SKIPPED UPDATE CTR" << std::endl;
}

void Solvers::Gurobi::update_obj() {

    const auto& objective = model().get(Attr::Obj::Expr);
    const auto sense = model().get(Attr::Obj::Sense) == Minimize ? GRB_MINIMIZE : GRB_MAXIMIZE;

    if (objective.quadratic().empty()) {

        GRBLinExpr expr = value(objective.constant());

        for (const auto& [var, constant] : objective.linear()) {
            expr += value(constant) * future(var).impl();
        }

        m_model.setObjective(expr, sense);

    } else {

        GRBQuadExpr expr = value(objective.constant());

        for (const auto& [var, constant] : objective.linear()) {
            expr.addTerm(value(constant), future(var).impl());
        }

        for (const auto& [var1, var2, constant] : objective.quadratic()) {
            expr.addTerm(value(constant), future(var1).impl(), future(var2).impl());
        }

        m_model.setObjective(expr, sense);

    }
}

#endif
