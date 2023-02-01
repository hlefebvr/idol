//
// Created by henri on 31/01/23.
//
#ifdef IDOL_USE_GUROBI

#include "backends/solvers/Gurobi.h"
#include "algorithms/parameters/Logs.h"

std::unique_ptr<GRBEnv> Gurobi::s_global_env;

GRBEnv &Gurobi::get_global_env() {
    if (!s_global_env) {
        s_global_env = std::make_unique<GRBEnv>();
    }
    return *s_global_env;
}

char Gurobi::gurobi_var_type(int t_type) {
    switch (t_type) {
        case Continuous: return GRB_CONTINUOUS;
        case Integer: return GRB_INTEGER;
        case Binary: return GRB_BINARY;
        default:;
    }
    throw Exception("Unsupported variable type: " + std::to_string(t_type));
}

char Gurobi::gurobi_ctr_type(int t_type) {
    switch (t_type) {
        case Equal: return GRB_EQUAL;
        case LessOrEqual: return GRB_LESS_EQUAL;
        case GreaterOrEqual: return GRB_GREATER_EQUAL;
        default:;
    }
    throw Exception("Unsupported constraint type: " + std::to_string(t_type));
}

char Gurobi::gurobi_obj_sense(int t_sense) {
    switch (t_sense) {
        case Minimize: return GRB_MINIMIZE;
        case Maximize: return GRB_MAXIMIZE;
        default:;
    }
    throw Exception("Unsupported objective sense: " + std::to_string(t_sense));
}

std::pair<char, char> Gurobi::gurobi_status(int t_status) const {
    switch (t_status) {
        case GRB_OPTIMAL: return { Optimal, Proved };
        case GRB_INFEASIBLE: return { Infeasible, Proved };
        case GRB_INF_OR_UNBD: return { InfeasibleOrUnbounded, Proved };
        case GRB_UNBOUNDED: return { Unbounded, Proved };
        case GRB_CUTOFF: return { Unknown, CutOff };
        case GRB_USER_OBJ_LIMIT: return { Feasible, UserObjLimit };
        case GRB_TIME_LIMIT: return { m_model.get(GRB_IntAttr_SolCount) > 0 ? Feasible : Infeasible, TimeLimit };
        case GRB_NUMERIC: return { Fail, NotSpecified };
        default:;
    }
    throw Exception("Unsupported status: " + std::to_string(t_status));
}
double Gurobi::gurobi_numeric(double t_value) {
    if (is_pos_inf(t_value)) {
        return GRB_INFINITY;
    }
    if (is_neg_inf(t_value)) {
        return -GRB_INFINITY;
    }
    return t_value;
}

void Gurobi::hook_initialize() {

    const auto& model = parent();
    const auto& objective = model.get(Attr::Obj::Expr);

    if (objective.quadratic().empty()) {
        hook_update_objective_sense();
        set_objective_as_updated();
    }

    set_rhs_as_updated();

}

GRBVar Gurobi::hook_add(const Var& t_var, bool t_add_column) {

    const auto& model = parent();
    const auto& column = model.get(Attr::Var::Column, t_var);
    const auto lb = model.get(Attr::Var::Lb, t_var);
    const auto ub = model.get(Attr::Var::Ub, t_var);
    const auto objective = as_numeric(column.obj());
    const auto type = gurobi_var_type(model.get(Attr::Var::Type, t_var));
    const auto& name = t_var.name();

    GRBColumn col;
    if (t_add_column) {

        for (const auto& [ctr, constant] : column.linear()) {

            auto& impl = lazy(ctr).impl();

            if (std::holds_alternative<GRBQConstr>(impl)) {
                throw Exception("Cannot add column to quadratic constraints.");
            }

            col.addTerm( as_numeric(constant), std::get<GRBConstr>(impl) );

        }

        if (!column.quadratic().empty()) {
            throw Exception("Cannot add column with quadratic terms.");
        }
    }

    return m_model.addVar(lb, ub, objective, type, col, name);
}

std::variant<GRBConstr, GRBQConstr> Gurobi::hook_add(const Ctr& t_ctr) {

    const auto& model = parent();
    const auto& row = model.get(Attr::Ctr::Row, t_ctr);
    const auto type = gurobi_ctr_type(model.get(Attr::Ctr::Type, t_ctr));
    const auto rhs = as_numeric(row.rhs());
    const auto& name = t_ctr.name();


    if (row.quadratic().empty()) {

        GRBLinExpr expr = 0.;
        for (const auto &[var, constant]: row.linear()) {
            expr += as_numeric(constant) * lazy(var).impl();
        }
        return m_model.addConstr(expr, type, rhs, name);

    }

    GRBQuadExpr expr = 0.;
    for (const auto &[var, constant]: row.linear()) {
        expr.addTerm(as_numeric(constant), lazy(var).impl());
    }
    for (const auto& [var1, var2, constant] : row.quadratic()) {
        expr.addTerm(as_numeric(constant), lazy(var1).impl(), lazy(var2).impl());
    }

    return m_model.addQConstr(expr, type, rhs, name);
}

void Gurobi::hook_update(const Var& t_var) {

    const auto& model = parent();
    auto& impl = lazy(t_var).impl();
    const double lb = model.get(Attr::Var::Lb, t_var);
    const double ub = model.get(Attr::Var::Ub, t_var);
    const int type = model.get(Attr::Var::Type, t_var);
    const Constant& obj = model.get(Attr::Var::Obj, t_var);

    impl.set(GRB_DoubleAttr_LB, gurobi_numeric(lb));
    impl.set(GRB_DoubleAttr_UB, gurobi_numeric(ub));
    impl.set(GRB_CharAttr_VType, gurobi_var_type(type));
    impl.set(GRB_DoubleAttr_Obj, gurobi_numeric(as_numeric(obj)));

}

void Gurobi::hook_update(const Ctr& t_ctr) {

    const auto& model = parent();
    auto& impl = lazy(t_ctr).impl();

    if (std::holds_alternative<GRBConstr>(impl)) {

        auto& linear_impl = std::get<GRBConstr>(impl);
        const auto& rhs = model.get(Attr::Ctr::Rhs, t_ctr);
        const auto type = model.get(Attr::Ctr::Type, t_ctr);

        linear_impl.set(GRB_DoubleAttr_RHS, gurobi_numeric(as_numeric(rhs)));
        linear_impl.set(GRB_CharAttr_Sense, gurobi_ctr_type(type));

    } else {
        throw Exception("Updating an SOCP constraint is not implemented.");
    }

}

void Gurobi::hook_update_objective() {

    const auto& model = parent();
    const auto& objective = model.get(Attr::Obj::Expr);
    const auto sense = model.get(Attr::Obj::Sense);

    GRBLinExpr linear_expr = gurobi_numeric(as_numeric(objective.constant()));

    for (const auto& [var, constant] : objective.linear()) {
        linear_expr += gurobi_numeric(as_numeric(constant)) * lazy(var).impl();
    }

    if (objective.quadratic().empty()) {
        m_model.setObjective(linear_expr, sense);
        return;
    }

    GRBQuadExpr quadratic_expr(linear_expr);

    for (const auto& [var, constant] : objective.linear()) {
        quadratic_expr.addTerm(gurobi_numeric(as_numeric(constant)), lazy(var).impl());
    }

    for (const auto& [var1, var2, constant] : objective.quadratic()) {
        quadratic_expr.addTerm(gurobi_numeric(as_numeric(constant)), lazy(var1).impl(), lazy(var2).impl());
    }

    m_model.setObjective(quadratic_expr, sense);


}

void Gurobi::hook_update_rhs() {

    const auto& model = parent();

    for (const auto& ctr : model.ctrs()) {
        auto& impl = lazy(ctr).impl();
        if (std::holds_alternative<GRBConstr>(impl)) {
            const auto& rhs = model.get(Attr::Ctr::Rhs, ctr);
            std::get<GRBConstr>(impl).set(GRB_DoubleAttr_RHS, gurobi_numeric(as_numeric(rhs)));
        } else {
            idol_Log(Warn, Gurobi, "Updating RHS on an SOCP constraint was skipped.")
        }
    }

}

void Gurobi::hook_remove(const Var& t_var) {

    const auto& impl = lazy(t_var).impl();
    m_model.remove(impl);

}

void Gurobi::hook_remove(const Ctr& t_ctr) {

    const auto& impl = lazy(t_ctr).impl();

    if (std::holds_alternative<GRBConstr>(impl)) {
        m_model.remove(std::get<GRBConstr>(impl));
    } else {
        m_model.remove(std::get<GRBQConstr>(impl));
    }

}

void Gurobi::hook_optimize() {
    m_model.optimize();
}

void Gurobi::hook_write(const std::string &t_name) {
    m_model.write(t_name);
}

void Gurobi::hook_update_objective_sense() {
    m_model.set(GRB_IntAttr_ModelSense, gurobi_obj_sense(parent().get(Attr::Obj::Sense)));
}

void Gurobi::hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) {

    const auto& var_impl = lazy(t_var).impl();
    const auto& ctr_impl = std::get<GRBConstr>(lazy(t_ctr).impl());

    m_model.chgCoeff(ctr_impl, var_impl, gurobi_numeric(as_numeric(t_constant)));

}

void Gurobi::hook_update() {
    m_model.update();
}

int Gurobi::get(const Req<int, void> &t_attr) const {

    if (t_attr == Attr::Solution::Status) {
        return gurobi_status(m_model.get(GRB_IntAttr_Status)).first;
    }

    if (t_attr == Attr::Solution::Reason) {
        return gurobi_status(m_model.get(GRB_IntAttr_Status)).second;
    }

    return Base::get(t_attr);
}

double Gurobi::get(const Req<double, void> &t_attr) const {

    if (t_attr == Attr::Solution::ObjVal) {
        return m_model.get(GRB_DoubleAttr_ObjVal);
    }

    return Base::get(t_attr);
}

double Gurobi::get(const Req<double, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Var::Value) {
        return lazy(t_var).impl().get(GRB_DoubleAttr_X);
    }

    if (t_attr == Attr::Var::RedCost) {
        return lazy(t_var).impl().get(GRB_DoubleAttr_RC);
    }

    return Base::get(t_attr, t_var);
}

double Gurobi::get(const Req<double, Ctr> &t_attr, const Ctr &t_ctr) const {

    if (t_attr == Attr::Ctr::Value) {
        const auto& impl = lazy(t_ctr).impl();
        if (std::holds_alternative<GRBConstr>(impl)) {
            return std::get<GRBQConstr>(impl).get(GRB_DoubleAttr_Pi);
        } else {
            return std::get<GRBConstr>(impl).get(GRB_DoubleAttr_Pi);
        }
    }

    if (t_attr == Attr::Ctr::Slack) {
        const auto& impl = lazy(t_ctr).impl();
        if (std::holds_alternative<GRBConstr>(impl)) {
            return std::get<GRBQConstr>(impl).get(GRB_DoubleAttr_Slack);
        } else {
            return std::get<GRBConstr>(impl).get(GRB_DoubleAttr_QCSlack);
        }
    }

    return Base::get(t_attr, t_ctr);
}

#endif