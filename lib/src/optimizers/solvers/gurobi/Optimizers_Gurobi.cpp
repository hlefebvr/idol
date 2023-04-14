//
// Created by henri on 31/01/23.
//
#ifdef IDOL_USE_GUROBI

#include "optimizers/solvers/gurobi/Optimizers_Gurobi.h"
#include "optimizers/Logger.h"

std::unique_ptr<GRBEnv> Optimizers::Gurobi::s_global_env;

GRBEnv &Optimizers::Gurobi::get_global_env() {
    if (!s_global_env) {
        s_global_env = std::make_unique<GRBEnv>();
    }
    return *s_global_env;
}

char Optimizers::Gurobi::gurobi_var_type(int t_type) {

    if (m_continuous_relaxation) {
        return GRB_CONTINUOUS;
    }

    switch (t_type) {
        case Continuous: return GRB_CONTINUOUS;
        case Integer: return GRB_INTEGER;
        case Binary: return GRB_BINARY;
        default:;
    }
    throw Exception("Unsupported variable type: " + std::to_string(t_type));
}

char Optimizers::Gurobi::gurobi_ctr_type(int t_type) {
    switch (t_type) {
        case Equal: return GRB_EQUAL;
        case LessOrEqual: return GRB_LESS_EQUAL;
        case GreaterOrEqual: return GRB_GREATER_EQUAL;
        default:;
    }
    throw Exception("Unsupported constraint type: " + std::to_string(t_type));
}

char Optimizers::Gurobi::gurobi_obj_sense(int t_sense) {
    switch (t_sense) {
        case Minimize: return GRB_MINIMIZE;
        case Maximize: return GRB_MAXIMIZE;
        default:;
    }
    throw Exception("Unsupported objective sense: " + std::to_string(t_sense));
}

std::pair<SolutionStatus, SolutionReason> Optimizers::Gurobi::gurobi_status(int t_status) const {
    switch (t_status) {
        case GRB_OPTIMAL: return { Optimal, Proved };
        case GRB_INFEASIBLE: return { Infeasible, Proved };
        case GRB_INF_OR_UNBD: return {InfOrUnbnd, Proved };
        case GRB_UNBOUNDED: return { Unbounded, Proved };
        case GRB_USER_OBJ_LIMIT: return {Feasible, ObjLimit };
        case GRB_TIME_LIMIT: return { m_model.get(GRB_IntAttr_SolCount) > 0 ? Feasible : Infeasible, TimeLimit };
        case GRB_NUMERIC: return {Fail, NotSpecified };
        default:;
    }
    throw Exception("Unsupported gurobi status: " + std::to_string(t_status));
}

double Optimizers::Gurobi::gurobi_numeric(double t_value) {
    if (is_pos_inf(t_value)) {
        return GRB_INFINITY;
    }
    if (is_neg_inf(t_value)) {
        return -GRB_INFINITY;
    }
    return t_value;
}

Optimizers::Gurobi::Gurobi(const Model &t_model, bool t_continuous_relaxation, GRBEnv &t_env)
    : OptimizerWithLazyUpdates(t_model),
      m_continuous_relaxation(t_continuous_relaxation),
      m_env(t_env),
      m_model(t_env) {

    m_model.set(GRB_IntParam_OutputFlag, 0);
    m_model.set(GRB_IntParam_QCPDual, 1);

}

void Optimizers::Gurobi::hook_build() {

    const auto& model = parent();
    const auto& objective = model.get_obj_expr();

    if (objective.quadratic().empty()) {
        hook_update_objective_sense();
        set_objective_as_updated();
    }

    set_rhs_as_updated();

}

GRBVar Optimizers::Gurobi::hook_add(const Var& t_var, bool t_add_column) {

    const auto& model = parent();
    const auto& column = model.get_var_column(t_var);
    const auto lb = model.get_var_lb(t_var);
    const auto ub = model.get_var_ub(t_var);
    const auto objective = as_numeric(column.obj());
    const auto type = gurobi_var_type(model.get_var_type(t_var));
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

std::variant<GRBConstr, GRBQConstr> Optimizers::Gurobi::hook_add(const Ctr& t_ctr) {

    const auto& model = parent();
    const auto& row = model.get_ctr_row(t_ctr);
    const auto type = gurobi_ctr_type(model.get_ctr_type(t_ctr));
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

void Optimizers::Gurobi::hook_update(const Var& t_var) {

    const auto& model = parent();
    auto& impl = lazy(t_var).impl();
    const double lb = model.get_var_lb(t_var);
    const double ub = model.get_var_ub(t_var);
    const int type = model.get_var_type(t_var);
    const Constant& obj = model.get_var_column(t_var).obj();

    impl.set(GRB_DoubleAttr_LB, gurobi_numeric(lb));
    impl.set(GRB_DoubleAttr_UB, gurobi_numeric(ub));
    impl.set(GRB_CharAttr_VType, gurobi_var_type(type));
    impl.set(GRB_DoubleAttr_Obj, gurobi_numeric(as_numeric(obj)));

}

void Optimizers::Gurobi::hook_update(const Ctr& t_ctr) {

    const auto& model = parent();
    auto& impl = lazy(t_ctr).impl();

    if (std::holds_alternative<GRBConstr>(impl)) {

        auto& linear_impl = std::get<GRBConstr>(impl);
        const auto& rhs = model.get_ctr_row(t_ctr).rhs();
        const auto type = model.get_ctr_type(t_ctr);

        linear_impl.set(GRB_DoubleAttr_RHS, gurobi_numeric(as_numeric(rhs)));
        linear_impl.set(GRB_CharAttr_Sense, gurobi_ctr_type(type));

    } else {
        throw Exception("Updating an SOCP constraint is not implemented.");
    }

}

void Optimizers::Gurobi::hook_update_objective() {

    const auto& model = parent();
    const auto& objective = model.get_obj_expr();
    const auto sense = gurobi_obj_sense(model.get_obj_sense());

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

void Optimizers::Gurobi::hook_update_rhs() {

    const auto& model = parent();

    for (const auto& ctr : model.ctrs()) {
        auto& impl = lazy(ctr).impl();
        if (std::holds_alternative<GRBConstr>(impl)) {
            const auto& rhs = model.get_ctr_row(ctr).rhs();
            std::get<GRBConstr>(impl).set(GRB_DoubleAttr_RHS, gurobi_numeric(as_numeric(rhs)));
        } else {
            idol_Log(Warn, "Updating RHS on an SOCP constraint was skipped.")
        }
    }

}

void Optimizers::Gurobi::hook_remove(const Var& t_var) {

    const auto& impl = lazy(t_var).impl();
    m_model.remove(impl);

}

void Optimizers::Gurobi::hook_remove(const Ctr& t_ctr) {

    const auto& impl = lazy(t_ctr).impl();

    if (std::holds_alternative<GRBConstr>(impl)) {
        m_model.remove(std::get<GRBConstr>(impl));
    } else {
        m_model.remove(std::get<GRBQConstr>(impl));
    }

}

void Optimizers::Gurobi::hook_optimize() {
    set_solution_index(0);
    m_model.optimize();
}

void Optimizers::Gurobi::hook_write(const std::string &t_name) {
    m_model.write(t_name);
}

void Optimizers::Gurobi::hook_update_objective_sense() {
    m_model.set(GRB_IntAttr_ModelSense, gurobi_obj_sense(parent().get_obj_sense()));
}

void Optimizers::Gurobi::hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) {

    const auto& var_impl = lazy(t_var).impl();
    const auto& ctr_impl = std::get<GRBConstr>(lazy(t_ctr).impl());

    m_model.chgCoeff(ctr_impl, var_impl, gurobi_numeric(as_numeric(t_constant)));

}

void Optimizers::Gurobi::hook_update() {
    m_model.update();
}

void Optimizers::Gurobi::set_time_limit(double t_time_limit) {
    m_model.set(GRB_DoubleParam_TimeLimit, t_time_limit);
    Optimizer::set_time_limit(t_time_limit);
}

void Optimizers::Gurobi::set_thread_limit(unsigned int t_thread_limit) {
    m_model.set(GRB_IntParam_Threads, t_thread_limit);
    Optimizer::set_thread_limit(t_thread_limit);
}

void Optimizers::Gurobi::set_best_obj_stop(double t_best_obj_stop) {
    m_model.set(GRB_DoubleParam_BestObjStop, t_best_obj_stop);
    Optimizer::set_best_obj_stop(t_best_obj_stop);
}

void Optimizers::Gurobi::set_best_bound_stop(double t_best_bound_stop) {
    m_model.set(GRB_DoubleParam_BestBdStop, t_best_bound_stop);
    Optimizer::set_best_bound_stop(t_best_bound_stop);
}

void Optimizers::Gurobi::set_presolve(bool t_value) {
    m_model.set(GRB_IntParam_Presolve, t_value);
    Optimizer::set_presolve(t_value);
}

void Optimizers::Gurobi::set_infeasible_or_unbounded_info(bool t_value) {
    m_model.set(GRB_IntParam_InfUnbdInfo, t_value);
    Optimizer::set_infeasible_or_unbounded_info(t_value);
}

void Optimizers::Gurobi::add_callback(Callback *t_ptr_to_callback) {

    if (!m_gurobi_callback) {
        m_gurobi_callback = std::make_unique<GurobiCallbackI>(*this);
        m_model.setCallback(m_gurobi_callback.get());
    }

    m_gurobi_callback->add_callback(t_ptr_to_callback);
}

void Optimizers::Gurobi::set_lazy_cut(bool t_value) {
    m_model.set(GRB_IntParam_LazyConstraints, t_value);
}

SolutionStatus Optimizers::Gurobi::get_status() const {
    return gurobi_status(m_model.get(GRB_IntAttr_Status)).first;
}

SolutionReason Optimizers::Gurobi::get_reason() const {
    return gurobi_status(m_model.get(GRB_IntAttr_Status)).second;
}

double Optimizers::Gurobi::get_best_obj() const {

    const auto status = gurobi_status(m_model.get(GRB_IntAttr_Status)).first;

    if (status == Unbounded) {
        return -Inf;
    }

    if (status == Infeasible) {
        return +Inf;
    }

    if (m_model.get(GRB_IntParam_SolutionNumber) == 0) {
        return m_model.get(GRB_DoubleAttr_ObjVal);
    }

    return m_model.get(GRB_DoubleAttr_PoolObjVal);

}

double Optimizers::Gurobi::get_best_bound() const {

    const auto status = gurobi_status(m_model.get(GRB_IntAttr_Status)).first;

    if (status == Unbounded) {
        return -Inf;
    }

    if (status == Infeasible) {
        return +Inf;
    }

    if (m_model.get(GRB_IntParam_SolutionNumber) == 0) {
        return m_model.get(GRB_DoubleAttr_ObjBound);
    }

    return m_model.get(GRB_DoubleAttr_PoolObjBound);

}

double Optimizers::Gurobi::get_var_primal(const Var &t_var) const {

    if (m_model.get(GRB_IntParam_SolutionNumber) == 0) {
        return lazy(t_var).impl().get(GRB_DoubleAttr_X);
    }

    return lazy(t_var).impl().get(GRB_DoubleAttr_Xn);
}

double Optimizers::Gurobi::get_var_ray(const Var &t_var) const {
    return lazy(t_var).impl().get(GRB_DoubleAttr_UnbdRay);
}

double Optimizers::Gurobi::get_ctr_dual(const Ctr &t_ctr) const {
    const auto& impl = lazy(t_ctr).impl();

    if (std::holds_alternative<GRBConstr>(impl)) {
        return std::get<GRBConstr>(impl).get(GRB_DoubleAttr_Pi);
    }

    return std::get<GRBQConstr>(impl).get(GRB_DoubleAttr_QCPi);
}

double Optimizers::Gurobi::get_ctr_farkas(const Ctr &t_ctr) const {
    const auto& impl = lazy(t_ctr).impl();

    if (!std::holds_alternative<GRBConstr>(impl)) {
        throw Exception("Gurobi does not handle Farkas certificates with quadratic constraints.");
    }

    return -std::get<GRBConstr>(impl).get(GRB_DoubleAttr_FarkasDual);
}

double Optimizers::Gurobi::get_relative_gap() const {
    throw Exception("Not implemented");
}

double Optimizers::Gurobi::get_absolute_gap() const {
    throw Exception("Not implemented");
}

unsigned int Optimizers::Gurobi::get_n_solutions() const {
    return m_model.get(GRB_IntAttr_SolCount);
}

unsigned int Optimizers::Gurobi::get_solution_index() const {
    return m_model.get(GRB_IntParam_SolutionNumber);
}

void Optimizers::Gurobi::set_solution_index(unsigned int t_index) {
    m_model.set(GRB_IntParam_SolutionNumber, (int) t_index);
}

#endif