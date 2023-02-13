//
// Created by henri on 11/10/22.
//
#include "../../../include/algorithms/solvers/Solvers_Gurobi.h"
#include "algorithms/branch-and-bound/BranchAndBound_Events.h"

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

void Solvers::Gurobi::execute() {

    m_is_in_callback = false;

    update();

    if (Algorithm::has_callback()) {
        m_callback = std::make_unique<CallbackProxy>(*this);
        m_model.set(GRB_IntParam_LazyConstraints, 1);
        m_model.setCallback(m_callback.get());
    }

    if (get(Param::Algorithm::ResetBeforeSolving)) {
        m_model.reset();
    }

    m_model.set(GRB_DoubleParam_TimeLimit, get(Param::Algorithm::TimeLimit));

    m_model.optimize();

    if (m_model.get(GRB_IntAttr_Status) == GRB_INFEASIBLE && m_model.get(GRB_IntParam_InfUnbdInfo)) {
        m_model.computeIIS();
    }

    analyze_status(m_model.get(GRB_IntAttr_Status));

}

void Solvers::Gurobi::analyze_status(int t_status) {

    switch (t_status) {
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
            throw Exception("Gurobi returned with status " + std::to_string(t_status) + " which is not handled.");
    }
}

char Solvers::Gurobi::gurobi_ctr_type(int t_type) {
    if (t_type == Equal) {
        return GRB_EQUAL;
    }
    if (t_type == LessOrEqual) {
        return GRB_LESS_EQUAL;
    }
    if (t_type == GreaterOrEqual) {
        return GRB_GREATER_EQUAL;
    }
    throw Exception("Unknown constraint type " + std::to_string(t_type));
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
        result.set_objective_value(unbounded_objective_value());
        return result;
    }

    if (t_status == Infeasible) {
        result.set_objective_value(infeasible_objective_value());
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

    if (m_is_in_callback) {

        switch (m_callback->where) {
            case GRB_CB_MIPSOL:
                return primal_solution(
                        Optimal,
                        Proved,
                        [this]() { return m_callback->getDoubleInfo(GRB_CB_MIPSOL_OBJ); },
                        [this](const GRBVar &t_var) { return m_callback->getSolution(t_var); }
                );
            case GRB_CB_MIPNODE: {
                auto result = primal_solution(
                        Optimal,
                        Proved,
                        [this]() { return 0.; },
                        [this](const GRBVar &t_var) { return m_callback->getNodeRel(t_var); }
                );

                const auto &objective = model().get(Attr::Obj::Expr);
                double value = objective.constant().numerical();
                for (const auto &[var, coeff]: objective.linear()) {
                    value += result.get(var) * coeff.numerical();
                }
                std::cout << "Computed = " << value << std::endl;

                result.set_objective_value(value);
                return result;
            }
            default:
                throw NotImplemented("Retrieving primal solution from callback with where = " + std::to_string(m_callback->where),
                                     "Solvers::Gurobi::primal_solution");
        }

    }

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
        result.set_objective_value(infeasible_objective_value());
        return result;
    }

    if (is_in(dual_status, { Infeasible })) {
        result.set_objective_value(unbounded_objective_value());
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
            [this](){ return infeasible_objective_value(); },
            [](const std::variant<GRBConstr, GRBQConstr>& t_ctr){

                if (std::holds_alternative<GRBConstr>(t_ctr)) {
                    return std::get<GRBConstr>(t_ctr).get(GRB_IntAttr_IISConstr);
                }
                return std::get<GRBQConstr>(t_ctr).get(GRB_IntAttr_IISQConstr);

            }
    );
}

void Solvers::Gurobi::write(const std::string &t_filename) {
    update();
    m_model.write(t_filename + ".lp");
}

void Solvers::Gurobi::execute_iis() {
    update();
    m_model.set(GRB_IntParam_IISMethod, 0);
    m_model.set(GRB_DoubleParam_Cutoff, GRB_INFINITY);
    m_model.set(GRB_DoubleParam_TimeLimit, get(Param::Algorithm::TimeLimit));


    m_model.computeIIS();
}

std::variant<GRBConstr, GRBQConstr> Solvers::Gurobi::create(const Ctr &t_ctr, bool t_with_collaterals) {

    const auto& row = model().get(Attr::Ctr::Row, t_ctr);
    const auto type = gurobi_ctr_type(model().get(Attr::Ctr::Type, t_ctr));
    const auto rhs = value(row.rhs());
    const auto& name = t_ctr.name();


    if (t_with_collaterals) {

        if (row.quadratic().empty()) {

            GRBLinExpr expr = 0.;
            for (const auto &[var, constant]: row.linear()) {
                expr += value(constant) * future(var).impl();
            }
            return m_model.addConstr(expr, type, rhs, name);

        }

        GRBQuadExpr expr = 0.;
        for (const auto &[var, constant]: row.linear()) {
            expr.addTerm(value(constant), future(var).impl());
        }
        for (const auto& [var1, var2, constant] : row.quadratic()) {
            expr.addTerm(value(constant), future(var1).impl(), future(var2).impl());
        }
        return m_model.addQConstr(expr, type, rhs, name);

    }

    return m_model.addConstr(0, type, rhs, name);

}

GRBVar Solvers::Gurobi::create(const Var &t_var, bool t_with_collaterals) {

    const auto& column = get(Attr::Var::Column, t_var);
    const auto lb = get(Attr::Var::Lb, t_var);
    const auto ub = get(Attr::Var::Ub, t_var);
    const auto objective = value(column.obj());
    const auto type = gurobi_var_type(get(Attr::Var::Type, t_var));
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

void Solvers::Gurobi::update_rhs() {

    const auto& rhs = model().get(Attr::Rhs::Expr);

    for (const auto& [ctr, constant] : rhs) {
        auto& impl = future(ctr).impl();
        if (std::holds_alternative<GRBConstr>(impl)) {
            std::get<GRBConstr>(impl).set(GRB_DoubleAttr_RHS, value(constant));
        }
    }

}

void Solvers::Gurobi::set(const Parameter<double> &t_param, double t_value) {

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

    }

    Algorithm::set(t_param, t_value);

}

void Solvers::Gurobi::set(const Parameter<bool> &t_param, bool t_value) {

    if (t_param.is_in_section(Param::Sections::Algorithm)) {

        if (t_param == Param::Algorithm::InfeasibleOrUnboundedInfo) {
            m_model.set(GRB_IntParam_InfUnbdInfo, t_value);
        } else if (t_param == Param::Algorithm::Presolve) {
            m_model.set(GRB_IntParam_Presolve, t_value);
        }

    }

    Algorithm::set(t_param, t_value);

}

void Solvers::Gurobi::set(const Parameter<int> &t_param, int t_value) {

    if (t_param.is_in_section(Param::Sections::Algorithm)) {

        if (t_param == Param::Algorithm::MaxThreads) {
            m_model.set(GRB_IntParam_Threads, t_value);
        }

    }

    Algorithm::set(t_param, t_value);
}

void
Solvers::Gurobi::set(const AttributeWithTypeAndArguments<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) {

    if (t_attr == Attr::Ctr::Rhs) {

        if (auto& future = this->future(t_ctr) ; future.has_impl()) {

            auto &impl = future.impl();

            if (std::holds_alternative<GRBConstr>(impl)) {
                std::get<GRBConstr>(impl).set(GRB_DoubleAttr_RHS, value(t_value));
            } else {
                std::get<GRBQConstr>(impl).set(GRB_DoubleAttr_QCRHS, value(t_value));
            }

        }

        model().set(Attr::Ctr::Rhs, t_ctr, std::move(t_value));

        return;

    }

    Delegate::set(t_attr, t_ctr, t_value);
}

void Solvers::Gurobi::set(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) {

    if (t_attr == Attr::Var::Lb || t_attr == Attr::Var::Ub) {

        auto gurobi_attr = t_attr == Attr::Var::Lb ? GRB_DoubleAttr_LB : GRB_DoubleAttr_UB;

        if (model().get(Attr::Var::Status, t_var)) {

            double value = t_value;
            if (is_pos_inf(value)) {
                value = GRB_INFINITY;
            } else if (is_neg_inf(value)) {
                value = -GRB_INFEASIBLE;
            }

            future(t_var).impl().set(gurobi_attr, value);
            model().set(t_attr, t_var, t_value);
            return;
        }

        for (auto& ptr_to_cb : m_callbacks) {
            ptr_to_cb->update_var_lb(t_var, t_value);
        }

        return;

    }

    Delegate::set(t_attr, t_var, t_value);
}

void Solvers::Gurobi::set(const AttributeWithTypeAndArguments<int, Var> &t_attr, const Var &t_var, int t_value) {

    if (t_attr == Attr::Var::Type) {

        future(t_var).impl().set(GRB_CharAttr_VType, gurobi_var_type(t_value));
        model().set(t_attr, t_var, t_value);
        return;

    }

    Delegate::set(t_attr, t_var, t_value);
}

double Solvers::Gurobi::get(const AttributeWithTypeAndArguments<double, void> &t_attr) const {

    if (t_attr == Attr::Solution::ObjVal) {

        if (status() == Unbounded) {
            return unbounded_objective_value();
        }

        if (status() == Infeasible) {
            return infeasible_objective_value();
        }

        if (m_is_in_callback) {

            switch (m_callback->where) {
                case GRB_CB_MIPSOL:
                    return m_callback->getDoubleInfo(GRB_CB_MIPSOL_OBJ);
                case GRB_CB_MIPNODE:
                    return primal_solution().objective_value();
                default:
                    throw NotImplemented("Retrieving primal objective value from callback with where = " + std::to_string(m_callback->where),
                                         "Solvers::Gurobi::get(Attr::Solution::ObjVal");
            }

        }
        return m_model.get(GRB_DoubleAttr_ObjVal);
    }

    return Delegate::get(t_attr);
}

Ctr Solvers::Gurobi::add_ctr(TempCtr&& t_temporary_constraint) {

    if (m_is_in_callback) {
        //update_variables();

        GRBLinExpr expr;
        for (const auto& [var, constant] : t_temporary_constraint.row().linear()) {
            expr += value(constant) * future(var).impl();
        }

        m_callback->addLazy(
                expr,
                gurobi_ctr_type(t_temporary_constraint.type()),
                value(t_temporary_constraint.row().rhs())
        );

        auto result = model().add_ctr(std::move(t_temporary_constraint));
        add_future(result);

        return result;
    }

    return Solver::add_ctr(std::move(t_temporary_constraint));
}

void Solvers::Gurobi::clear_rhs() {

    const auto& rhs = model().get(Attr::Rhs::Expr);

    for (const auto& [ctr, constant] : rhs) {
        auto& impl = future(ctr).impl();
        if (std::holds_alternative<GRBConstr>(impl)) {
            std::get<GRBConstr>(impl).set(GRB_DoubleAttr_RHS, 0.);
        }
    }

    for (const auto& [ctr, constant] : rhs) {
        model().set(Attr::Ctr::Rhs, ctr, 0.);
    }
}

int Solvers::Gurobi::sense() const {
    return model().get(Attr::Obj::Sense);
}

void Solvers::Gurobi::CallbackProxy::callback() {

    m_parent.m_is_in_callback = true;

    switch (where) {
        case GRB_CB_MIPSOL:
            m_parent.set_status(Optimal);
            m_parent.set_reason(Proved);
            m_parent.call_callback(Event_::BranchAndBound::NewIncumbentFound);
            break;
        case GRB_CB_MIPNODE:
            if (getIntInfo(GRB_CB_MIPNODE_STATUS) == GRB_OPTIMAL) {
                m_parent.analyze_status(getIntInfo(GRB_CB_MIPNODE_STATUS));
                m_parent.call_callback(Event_::BranchAndBound::RelaxationSolved);
            }
            break;
        default:;
    }

    m_parent.m_is_in_callback = false;

}

#endif
