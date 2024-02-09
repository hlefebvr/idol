//
// Created by henri on 31/01/23.
//
#ifdef IDOL_USE_GUROBI

#include "idol/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"
#include "idol/modeling/expressions/operations/operators.h"

#define GUROBI_CATCH(cmd) \
try { \
    cmd \
} catch (GRBException& error) { \
throw Exception("Gurobi exception [" + std::to_string(error.getErrorCode()) + "] " + error.getMessage() ); \
}

std::unique_ptr<GRBEnv> idol::Optimizers::Gurobi::s_global_env;

GRBEnv &idol::Optimizers::Gurobi::get_global_env() {
    if (!s_global_env) {
        s_global_env = std::make_unique<GRBEnv>();
    }
    return *s_global_env;
}

char idol::Optimizers::Gurobi::gurobi_var_type(int t_type) {

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

char idol::Optimizers::Gurobi::gurobi_ctr_type(int t_type) {
    switch (t_type) {
        case Equal: return GRB_EQUAL;
        case LessOrEqual: return GRB_LESS_EQUAL;
        case GreaterOrEqual: return GRB_GREATER_EQUAL;
        default:;
    }
    throw Exception("Unsupported constraint type: " + std::to_string(t_type));
}

char idol::Optimizers::Gurobi::gurobi_obj_sense(int t_sense) {
    switch (t_sense) {
        case Minimize: return GRB_MINIMIZE;
        case Maximize: return GRB_MAXIMIZE;
        default:;
    }
    throw Exception("Unsupported objective sense: " + std::to_string(t_sense));
}

std::pair<idol::SolutionStatus, idol::SolutionReason> idol::Optimizers::Gurobi::gurobi_status(int t_status) const {
    switch (t_status) {
        case GRB_SUBOPTIMAL: return { SubOptimal, Proved };
        case GRB_OPTIMAL: return { Optimal, Proved };
        case GRB_INFEASIBLE: return { Infeasible, Proved };
        case GRB_INF_OR_UNBD: return {InfOrUnbnd, Proved };
        case GRB_UNBOUNDED: return { Unbounded, Proved };
        case GRB_CUTOFF: [[fallthrough]];
        case GRB_USER_OBJ_LIMIT: return {Feasible, ObjLimit };
        case GRB_TIME_LIMIT: return { m_model.get(GRB_IntAttr_SolCount) > 0 ? Feasible : Infeasible, TimeLimit };
        case GRB_NUMERIC: return {Fail, NotSpecified };
        default:;
    }
    throw Exception("Unsupported Gurobi status: " + std::to_string(t_status));
}

double idol::Optimizers::Gurobi::gurobi_numeric(double t_value) {
    if (is_pos_inf(t_value)) {
        return GRB_INFINITY;
    }
    if (is_neg_inf(t_value)) {
        return -GRB_INFINITY;
    }
    return t_value;
}

idol::Optimizers::Gurobi::Gurobi(const Model &t_model, bool t_continuous_relaxation, GRBEnv &t_env)
    : OptimizerWithLazyUpdates(t_model),
      m_continuous_relaxation(t_continuous_relaxation),
      m_env(t_env),
      m_model(t_env) {

    // Parameters
    m_model.set(GRB_IntParam_OutputFlag, get_param_logs());
    m_model.set(GRB_DoubleParam_BestBdStop, get_param_best_bound_stop());
    m_model.set(GRB_DoubleParam_BestObjStop, get_param_best_obj_stop());
    m_model.set(GRB_DoubleParam_TimeLimit, get_param_time_limit());
    m_model.set(GRB_IntParam_Presolve, get_param_presolve());
    m_model.set(GRB_IntParam_Threads, (int) get_param_threads());

    // Tolerances
    m_model.set(GRB_DoubleParam_MIPGap, get_tol_mip_relative_gap());
    m_model.set(GRB_DoubleParam_MIPGapAbs, get_tol_mip_absolute_gap());
    m_model.set(GRB_DoubleParam_IntFeasTol, get_tol_integer());
    m_model.set(GRB_DoubleParam_FeasibilityTol, get_tol_feasibility());
    m_model.set(GRB_DoubleParam_OptimalityTol, get_tol_optimality());
}

void idol::Optimizers::Gurobi::hook_build() {

    const auto& model = parent();
    const auto& objective = model.get_obj_expr();

    if (objective.quadratic().empty()) {
        hook_update_objective_sense();
        update_objective_constant();
        set_objective_as_updated();
    }

    set_rhs_as_updated();

}

GRBVar idol::Optimizers::Gurobi::hook_add(const Var& t_var, bool t_add_column) {

    const auto& model = parent();
    const auto& column = model.get_var_column(t_var);
    const auto lb = model.get_var_lb(t_var);
    const auto ub = model.get_var_ub(t_var);
    const auto objective = column.obj().as_numerical();
    const auto type = gurobi_var_type(model.get_var_type(t_var));
    const auto& name = t_var.name();

    GRBColumn col;
    if (t_add_column) {

        for (const auto& [ctr, constant] : column.linear()) {

            auto& impl = lazy(ctr).impl();

            if (std::holds_alternative<GRBQConstr>(impl)) {
                throw Exception("Cannot add column to quadratic constraints.");
            }

            col.addTerm( constant.as_numerical(), std::get<GRBConstr>(impl) );

        }

        if (!column.quadratic().empty()) {
            throw Exception("Cannot add column with quadratic terms.");
        }
    }

    GUROBI_CATCH(return m_model.addVar(lb, ub, objective, type, col, name);)
}

std::variant<GRBConstr, GRBQConstr> idol::Optimizers::Gurobi::hook_add(const Ctr& t_ctr) {

    const auto& model = parent();
    const auto& row = model.get_ctr_row(t_ctr);
    const auto type = gurobi_ctr_type(model.get_ctr_type(t_ctr));
    const auto rhs = row.rhs().as_numerical();
    const auto& name = t_ctr.name();


    if (row.quadratic().empty()) {

        GRBLinExpr expr = 0.;
        for (const auto &[var, constant]: row.linear()) {
            expr += constant.as_numerical() * lazy(var).impl();
        }
        GUROBI_CATCH(return m_model.addConstr(expr, type, rhs, name);)

    }

    GRBQuadExpr expr = 0.;
    for (const auto &[var, constant]: row.linear()) {
        expr.addTerm(constant.as_numerical(), lazy(var).impl());
    }
    for (const auto& [var1, var2, constant] : row.quadratic()) {
        expr.addTerm(constant.as_numerical(), lazy(var1).impl(), lazy(var2).impl());
    }

    GUROBI_CATCH(return m_model.addQConstr(expr, type, rhs, name);)
}

void idol::Optimizers::Gurobi::hook_update(const Var& t_var) {

    const auto& model = parent();
    auto& impl = lazy(t_var).impl();
    const double lb = model.get_var_lb(t_var);
    const double ub = model.get_var_ub(t_var);
    const int type = model.get_var_type(t_var);
    const Constant& obj = model.get_var_column(t_var).obj();

    impl.set(GRB_DoubleAttr_LB, gurobi_numeric(lb));
    impl.set(GRB_DoubleAttr_UB, gurobi_numeric(ub));
    impl.set(GRB_CharAttr_VType, gurobi_var_type(type));
    impl.set(GRB_DoubleAttr_Obj, gurobi_numeric(obj.as_numerical()));

}

void idol::Optimizers::Gurobi::hook_update(const Ctr& t_ctr) {

    const auto& model = parent();
    auto& impl = lazy(t_ctr).impl();

    if (std::holds_alternative<GRBConstr>(impl)) {

        auto& linear_impl = std::get<GRBConstr>(impl);
        const auto& rhs = model.get_ctr_row(t_ctr).rhs();
        const auto type = model.get_ctr_type(t_ctr);

        linear_impl.set(GRB_DoubleAttr_RHS, gurobi_numeric(rhs.as_numerical()));
        linear_impl.set(GRB_CharAttr_Sense, gurobi_ctr_type(type));

    } else {
        throw Exception("Updating an SOCP constraint is not implemented.");
    }

}

void idol::Optimizers::Gurobi::hook_update_objective() {

    const auto& model = parent();
    const auto& objective = model.get_obj_expr();
    const auto sense = gurobi_obj_sense(model.get_obj_sense());

    GRBLinExpr linear_expr = gurobi_numeric(objective.constant().as_numerical());

    for (const auto& [var, constant] : objective.linear()) {
        linear_expr += gurobi_numeric(constant.as_numerical()) * lazy(var).impl();
    }

    if (objective.quadratic().empty()) {
        m_model.setObjective(linear_expr, sense);
        return;
    }

    GRBQuadExpr quadratic_expr;

    for (const auto& [var1, var2, constant] : objective.quadratic()) {
        quadratic_expr.addTerm(gurobi_numeric(constant.as_numerical()), lazy(var1).impl(), lazy(var2).impl());
    }

    m_model.setObjective(linear_expr + quadratic_expr, sense);


}

void idol::Optimizers::Gurobi::hook_update_rhs() {

    const auto& model = parent();

    for (const auto& ctr : model.ctrs()) {
        auto& impl = lazy(ctr).impl();
        if (std::holds_alternative<GRBConstr>(impl)) {
            const auto& rhs = model.get_ctr_row(ctr).rhs();
            std::get<GRBConstr>(impl).set(GRB_DoubleAttr_RHS, gurobi_numeric(rhs.as_numerical()));
        } else {
            std::cout << "Warning: Updating RHS on an SOCP constraint was skipped" << std::endl;
        }
    }

}

void idol::Optimizers::Gurobi::hook_remove(const Var& t_var) {

    const auto& impl = lazy(t_var).impl();
    m_model.remove(impl);

}

void idol::Optimizers::Gurobi::hook_remove(const Ctr& t_ctr) {

    const auto& impl = lazy(t_ctr).impl();

    if (std::holds_alternative<GRBConstr>(impl)) {
        m_model.remove(std::get<GRBConstr>(impl));
    } else {
        m_model.remove(std::get<GRBQConstr>(impl));
    }

}

void idol::Optimizers::Gurobi::hook_optimize() {
    set_solution_index(0);

    GUROBI_CATCH(
        m_model.optimize();
    )

}

void idol::Optimizers::Gurobi::hook_write(const std::string &t_name) {
    GUROBI_CATCH(m_model.write(t_name);)
}

void idol::Optimizers::Gurobi::hook_update_objective_sense() {
    m_model.set(GRB_IntAttr_ModelSense, gurobi_obj_sense(parent().get_obj_sense()));
}

void idol::Optimizers::Gurobi::hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) {

    const auto& var_impl = lazy(t_var).impl();
    const auto& ctr_impl = std::get<GRBConstr>(lazy(t_ctr).impl());

    m_model.chgCoeff(ctr_impl, var_impl, gurobi_numeric(t_constant.as_numerical()));

}

void idol::Optimizers::Gurobi::hook_update() {
    m_model.update();
}

void idol::Optimizers::Gurobi::set_param_time_limit(double t_time_limit) {
    m_model.set(GRB_DoubleParam_TimeLimit, t_time_limit);
    Optimizer::set_param_time_limit(t_time_limit);
}

void idol::Optimizers::Gurobi::set_param_threads(unsigned int t_thread_limit) {
    m_model.set(GRB_IntParam_Threads, t_thread_limit);
    Optimizer::set_param_threads(t_thread_limit);
}

void idol::Optimizers::Gurobi::set_param_best_obj_stop(double t_best_obj_stop) {
    m_model.set(GRB_DoubleParam_BestObjStop, t_best_obj_stop);
    Optimizer::set_param_best_obj_stop(t_best_obj_stop);
}

void idol::Optimizers::Gurobi::set_param_best_bound_stop(double t_best_bound_stop) {
    m_model.set(GRB_DoubleParam_BestBdStop, t_best_bound_stop);
    Optimizer::set_param_best_bound_stop(t_best_bound_stop);
}

void idol::Optimizers::Gurobi::set_param_presolve(bool t_value) {
    m_model.set(GRB_IntParam_Presolve, t_value);
    Optimizer::set_param_presolve(t_value);
}

void idol::Optimizers::Gurobi::set_param_infeasible_or_unbounded_info(bool t_value) {
    m_model.set(GRB_IntParam_InfUnbdInfo, t_value);
    Optimizer::set_param_infeasible_or_unbounded_info(t_value);
}

void idol::Optimizers::Gurobi::add_callback(Callback *t_ptr_to_callback) {

    if (!m_gurobi_callback) {
        m_gurobi_callback = std::make_unique<GurobiCallbackI>(*this);
        m_model.setCallback(m_gurobi_callback.get());
    }

    m_gurobi_callback->add_callback(t_ptr_to_callback);
}

void idol::Optimizers::Gurobi::set_lazy_cut(bool t_value) {
    m_model.set(GRB_IntParam_LazyConstraints, t_value);
}

idol::SolutionStatus idol::Optimizers::Gurobi::get_status() const {
    return gurobi_status(m_model.get(GRB_IntAttr_Status)).first;
}

idol::SolutionReason idol::Optimizers::Gurobi::get_reason() const {
    return gurobi_status(m_model.get(GRB_IntAttr_Status)).second;
}

double idol::Optimizers::Gurobi::get_best_obj() const {

    const auto status = gurobi_status(m_model.get(GRB_IntAttr_Status)).first;

    if (status == Unbounded) {
        return -Inf;
    }

    if (status == Infeasible) {
        return +Inf;
    }

    if (m_model.get(GRB_IntParam_SolutionNumber) == 0) {
        GUROBI_CATCH(return m_model.get(GRB_DoubleAttr_ObjVal);)
    }

    GUROBI_CATCH(return m_model.get(GRB_DoubleAttr_PoolObjVal);)

}

double idol::Optimizers::Gurobi::get_best_bound() const {

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

double idol::Optimizers::Gurobi::get_var_primal(const Var &t_var) const {

    if (m_model.get(GRB_IntParam_SolutionNumber) == 0) {
        GUROBI_CATCH(
            return lazy(t_var).impl().get(GRB_DoubleAttr_X);
        )
    }

    GUROBI_CATCH(
        return lazy(t_var).impl().get(GRB_DoubleAttr_Xn);
    )
}

double idol::Optimizers::Gurobi::get_var_ray(const Var &t_var) const {
    GUROBI_CATCH(
        return lazy(t_var).impl().get(GRB_DoubleAttr_UnbdRay);
    )
}

double idol::Optimizers::Gurobi::get_ctr_dual(const Ctr &t_ctr) const {
    const auto& impl = lazy(t_ctr).impl();

    if (std::holds_alternative<GRBConstr>(impl)) {
        GUROBI_CATCH(
            return std::get<GRBConstr>(impl).get(GRB_DoubleAttr_Pi);
        )
    }

    GUROBI_CATCH(
        return std::get<GRBQConstr>(impl).get(GRB_DoubleAttr_QCPi);
    )
}

double idol::Optimizers::Gurobi::get_ctr_farkas(const Ctr &t_ctr) const {
    const auto& impl = lazy(t_ctr).impl();

    if (!std::holds_alternative<GRBConstr>(impl)) {
        throw Exception("Gurobi does not handle Farkas certificates with quadratic constraints.");
    }

    GUROBI_CATCH(
        return -std::get<GRBConstr>(impl).get(GRB_DoubleAttr_FarkasDual);
    )
}

double idol::Optimizers::Gurobi::get_relative_gap() const {
    throw Exception("Not implemented");
}

double idol::Optimizers::Gurobi::get_absolute_gap() const {
    throw Exception("Not implemented");
}

unsigned int idol::Optimizers::Gurobi::get_n_solutions() const {
    return m_model.get(GRB_IntAttr_SolCount);
}

unsigned int idol::Optimizers::Gurobi::get_solution_index() const {
    return m_model.get(GRB_IntParam_SolutionNumber);
}

void idol::Optimizers::Gurobi::set_solution_index(unsigned int t_index) {
    m_model.set(GRB_IntParam_SolutionNumber, (int) t_index);
}

void idol::Optimizers::Gurobi::set_max_n_solution_in_pool(unsigned int t_value) {
    m_model.set(GRB_IntParam_PoolSolutions, (int) std::min<unsigned int>(GRB_MAXINT, t_value));
}

void idol::Optimizers::Gurobi::set_param_logs(bool t_value) {
    m_model.set(GRB_IntParam_OutputFlag, t_value);
    Optimizer::set_param_logs(t_value);
}

void idol::Optimizers::Gurobi::set_param(GRB_IntParam t_param, int t_value) {
    m_model.set(t_param, t_value);
}

void idol::Optimizers::Gurobi::set_param(GRB_DoubleParam t_param, double t_value) {
    m_model.set(t_param, t_value);
}

void idol::Optimizers::Gurobi::set_tol_mip_relative_gap(double t_relative_gap_tolerance) {
    Optimizer::set_tol_mip_relative_gap(t_relative_gap_tolerance);
    m_model.set(GRB_DoubleParam_MIPGap, t_relative_gap_tolerance);
}

void idol::Optimizers::Gurobi::set_tol_mip_absolute_gap(double t_absolute_gap_tolerance) {
    Optimizer::set_tol_mip_absolute_gap(t_absolute_gap_tolerance);
    m_model.set(GRB_DoubleParam_MIPGapAbs, t_absolute_gap_tolerance);
}

void idol::Optimizers::Gurobi::set_tol_feasibility(double t_tol_feasibility) {
    Optimizer::set_tol_feasibility(t_tol_feasibility);
    m_model.set(GRB_DoubleParam_FeasibilityTol, t_tol_feasibility);
}

void idol::Optimizers::Gurobi::set_tol_optimality(double t_tol_optimality) {
    Optimizer::set_tol_optimality(t_tol_optimality);
    m_model.set(GRB_DoubleParam_OptimalityTol, t_tol_optimality);
}

void idol::Optimizers::Gurobi::set_tol_integer(double t_tol_integer) {
    Optimizer::set_tol_integer(t_tol_integer);
    m_model.set(GRB_DoubleParam_IntFeasTol, t_tol_integer);
}

idol::Model idol::Optimizers::Gurobi::read_from_file(idol::Env &t_env, const std::string &t_filename) {

    Model result(t_env);

    std::unique_ptr<GRBModel> model;
    GUROBI_CATCH(model = std::make_unique<GRBModel>(get_global_env(), t_filename);)

    const unsigned int n_vars = model->get(GRB_IntAttr_NumVars);
    const unsigned int n_ctrs = model->get(GRB_IntAttr_NumConstrs);
    const unsigned int n_quad_ctrs = model->get(GRB_IntAttr_NumQConstrs);

    for (unsigned int j = 0 ; j < n_vars ; ++j) {

        const auto& var = model->getVar(j);
        const double lb = var.get(GRB_DoubleAttr_LB);
        const double ub = var.get(GRB_DoubleAttr_UB);
        VarType type = idol_var_type(var.get(GRB_CharAttr_VType));

        result.add_var(lb, ub, type, var.get(GRB_StringAttr_VarName));
    }

    const auto parse_linear = [&](const GRBLinExpr& t_lin_expr) {
        Expr result_ = t_lin_expr.getConstant();

        for (unsigned int j = 0, n = t_lin_expr.size() ; j < n ; ++j) {
            auto var = t_lin_expr.getVar(j);
            result_ += t_lin_expr.getCoeff(j) * result.get_var_by_index(var.index());
        }

        return result_;
    };

    const auto parse_quadratic = [&](const GRBQuadExpr& t_quad_expr) {
        Expr result_ = parse_linear(t_quad_expr.getLinExpr());

        for (unsigned int j = 0, n = t_quad_expr.size() ; j < n ; ++j) {
            auto var1 = t_quad_expr.getVar1(j);
            auto var2 = t_quad_expr.getVar2(j);
            result_ += t_quad_expr.getCoeff(j) * result.get_var_by_index(var1.index()) * result.get_var_by_index(var2.index());
        }

        return result_;
    };

    const auto add_ctr = [&](
            const auto& t_lhs,
            const auto& t_rhs,
            char t_type,
            const std::string& t_name) {

        switch (t_type) {
            case LessOrEqual: result.add_ctr(t_lhs <= t_rhs, t_name); break;
            case GreaterOrEqual: result.add_ctr(t_lhs >= t_rhs, t_name); break;
            case Equal: result.add_ctr(t_lhs == t_rhs, t_name); break;
            default: throw Exception("Enum out of bounds.");
        }

    };

    for (unsigned int i = 0 ; i < n_ctrs ; ++i) {

        const auto& ctr = model->getConstr(i);
        const auto& expr = model->getRow(ctr);
        const double rhs = ctr.get(GRB_DoubleAttr_RHS);
        const auto type = idol_ctr_type(ctr.get(GRB_CharAttr_Sense));
        const auto& name = ctr.get(GRB_StringAttr_ConstrName);

        Expr lhs = parse_linear(expr);
        add_ctr(lhs, rhs, type, name);
    }

    for (unsigned int i = 0 ; i < n_quad_ctrs ; ++i) {

        const auto& ctr = model->getQConstrs()[i];
        const auto& expr = model->getQCRow(ctr);
        const double rhs = ctr.get(GRB_DoubleAttr_QCRHS);
        const auto type = idol_ctr_type(ctr.get(GRB_CharAttr_QCSense));
        const auto& name = ctr.get(GRB_StringAttr_QCName);

        Expr lhs = parse_quadratic(expr);
        add_ctr(lhs, rhs, type, name);

    }

    const auto sense = model->get(GRB_IntAttr_ModelSense);
    model->set(GRB_IntAttr_ModelSense, idol_obj_sense(sense));

    const auto& objective = model->getObjective();
    result.set_obj_expr(parse_quadratic(objective));

    return std::move(result);
}

idol::VarType idol::Optimizers::Gurobi::idol_var_type(char t_type) {

    switch (t_type) {
        case GRB_INTEGER: return Integer;
        case GRB_BINARY: return Binary;
        case GRB_CONTINUOUS: return Continuous;
        default:;
    }

    throw Exception("Unexpected variable type.");
}

idol::CtrType idol::Optimizers::Gurobi::idol_ctr_type(char t_type) {

    switch (t_type) {
        case GRB_LESS_EQUAL: return LessOrEqual;
        case GRB_GREATER_EQUAL: return GreaterOrEqual;
        case GRB_EQUAL: return Equal;
        default:;
    }

    throw Exception("Unexpected constraint type.");
}

idol::ObjectiveSense idol::Optimizers::Gurobi::idol_obj_sense(int t_sense) {

    switch (t_sense) {
        case GRB_MINIMIZE: return Minimize;
        case GRB_MAXIMIZE: return Maximize;
        default:;
    }

    throw Exception("Unexpected constraint type.");
}

void idol::Optimizers::Gurobi::update_objective_constant() {
    const double constant = parent().get_obj_expr().constant().as_numerical();
    m_model.set(GRB_DoubleAttr_ObjCon, constant);
}

#endif