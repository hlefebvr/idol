//
// Created by henri on 31/01/23.
//
#ifdef IDOL_USE_GUROBI

#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

#define GUROBI_CATCH(model, cmd)                                \
{                                                           \
    int err = (cmd);                                          \
    if (err) {                                                         \
        throw Exception("Gurobi exception [" + std::to_string(err) +  \
        "] " + GRBgeterrormsg(GRBgetenv(model)));                   \
    }  \
}

GRBenv* idol::Optimizers::Gurobi::s_global_env = nullptr;

GRBenv* idol::Optimizers::Gurobi::get_global_env() {
    if (!s_global_env) {
        GRBloadenv(&s_global_env, nullptr);
    }
    return s_global_env;
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
        case GRB_TIME_LIMIT: {
            int n_solutions;
            GUROBI_CATCH(m_model, GRBgetintattr(m_model, "SolCount", &n_solutions));
            return { n_solutions > 0 ? Feasible : Infeasible, TimeLimit };
        }
        case GRB_NUMERIC: return {Fail, NotSpecified };
        case GRB_SOLUTION_LIMIT: return {Feasible, SolutionLimit };
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

idol::Optimizers::Gurobi::Gurobi(const Model &t_model, bool t_continuous_relaxation, GRBenv *t_env)
    : OptimizerWithLazyUpdates(t_model),
      m_continuous_relaxation(t_continuous_relaxation),
      m_env(t_env) {

    GRBnewmodel(t_env, &m_model, "idol-model", 0, nullptr, nullptr, nullptr, nullptr, nullptr);

}

void idol::Optimizers::Gurobi::hook_build() {

    const auto& model = parent();
    const auto& objective = model.get_obj_expr();

    if (!objective.has_quadratic()) {
        hook_update_objective_sense();
        update_objective_constant();
        set_objective_as_updated();
    }

    set_rhs_as_updated();

}

int idol::Optimizers::Gurobi::hook_add(const Var& t_var, bool t_add_column) {

    const auto& model = parent();
    const auto& column = model.get_var_column(t_var);
    const auto lb = model.get_var_lb(t_var);
    const auto ub = model.get_var_ub(t_var);
    const auto objective = model.get_var_obj(t_var);
    const auto type = gurobi_var_type(model.get_var_type(t_var));
    const auto& name = t_var.name();

    std::vector<int> indices;
    std::vector<double> values;

    if (t_add_column) {

        indices.reserve(column.size());
        values.reserve(column.size());

        for (const auto& [ctr, constant] : column) {

            if (!has_lazy(ctr)) { // if the constraint has no lazy, it will be created right after
                continue;
            }

            auto& impl = lazy(ctr).impl();
            indices.push_back(impl);
            values.push_back(gurobi_numeric(constant));

        }

    }

    const unsigned int index = model.get_var_index(t_var);

    GUROBI_CATCH(
        m_model,
        GRBaddvar(m_model,
            (int) indices.size(),
            indices.data(),
            values.data(),
            objective,
            lb,
            ub,
            type,
            name.c_str()
            )
    )

    return index;
}

int idol::Optimizers::Gurobi::hook_add(const Ctr& t_ctr) {

    const auto& model = parent();
    const auto& row = model.get_ctr_row(t_ctr);
    const auto type = gurobi_ctr_type(model.get_ctr_type(t_ctr));
    const auto rhs = model.get_ctr_rhs(t_ctr);
    const auto& name = t_ctr.name();

    std::vector<int> vars;
    std::vector<double> vals;
    vars.reserve(row.size());
    vals.reserve(row.size());

    for (const auto& [var, constant] : row) {
        vars.push_back(lazy(var).impl());
        vals.push_back(gurobi_numeric(constant));
    }

    const unsigned int index = model.get_ctr_index(t_ctr);

    GUROBI_CATCH(
        m_model,
        GRBaddconstr(
            m_model,
            (int) vars.size(),
            vars.data(),
            vals.data(),
            type,
            rhs,
            name.c_str()
        )
    )

    return index;
}

int idol::Optimizers::Gurobi::hook_add(const idol::QCtr &t_ctr) {

    const auto& model = parent();
    const auto& expr = model.get_qctr_expr(t_ctr);
    const auto type = gurobi_ctr_type(model.get_qctr_type(t_ctr));
    const auto& name = t_ctr.name();

    // Linear part
    const auto& linear_terms = expr.affine().linear();
    std::vector<int> linear_indices;
    std::vector<double> linear_values;
    linear_indices.reserve(linear_terms.size());
    linear_values.reserve(linear_terms.size());

    for (const auto& [var, coeff] : linear_terms) {
        linear_indices.push_back(lazy(var).impl());
        linear_values.push_back(gurobi_numeric(coeff));
    }

    /// Constant
    double linear_constant = gurobi_numeric(expr.affine().constant());

    // Quadratic part
    std::vector<int> quadratic_indices1, quadratic_indices2;
    std::vector<double> quadratic_values;
    quadratic_indices1.reserve(expr.size());
    quadratic_indices2.reserve(expr.size());
    quadratic_values.reserve(expr.size());

    for (const auto& [pair, coeff] : expr) {
        quadratic_indices1.push_back(lazy(pair.first).impl());
        quadratic_indices2.push_back(lazy(pair.second).impl());
        quadratic_values.push_back(gurobi_numeric(coeff));
    }

    const unsigned int index = model.get_qctr_index(t_ctr);

    GUROBI_CATCH(
        m_model,
        GRBaddqconstr(
            m_model,
            (int) linear_indices.size(),
            linear_indices.data(),
            linear_values.data(),
            (int) quadratic_indices1.size(),
            quadratic_indices1.data(),
            quadratic_indices2.data(),
            quadratic_values.data(),
            type,
            linear_constant,
            name.c_str()
        )
    )

    return index;
}

void idol::Optimizers::Gurobi::hook_update(const Var& t_var) {

    const auto& model = parent();
    int index = lazy(t_var).impl();
    const double lb = model.get_var_lb(t_var);
    const double ub = model.get_var_ub(t_var);
    const int type = model.get_var_type(t_var);
    const double obj = model.get_var_obj(t_var);

    GUROBI_CATCH(
        m_model,
        GRBsetdblattrelement(m_model, GRB_DBL_ATTR_LB, index, gurobi_numeric(lb))
    )

    GUROBI_CATCH(
        m_model,
        GRBsetdblattrelement(m_model, GRB_DBL_ATTR_UB, index, gurobi_numeric(ub))
    )

    GUROBI_CATCH(
        m_model,
        GRBsetcharattrelement(m_model, GRB_CHAR_ATTR_VTYPE, index, gurobi_var_type(type))
    )

    GUROBI_CATCH(
        m_model,
        GRBsetdblattrelement(m_model, GRB_DBL_ATTR_OBJ, index, gurobi_numeric(obj))
    )

}

void idol::Optimizers::Gurobi::hook_update(const Ctr& t_ctr) {

    const auto& model = parent();
    auto index = lazy(t_ctr).impl();

    const auto& rhs = model.get_ctr_rhs(t_ctr);
    const auto type = model.get_ctr_type(t_ctr);

    GUROBI_CATCH(
        m_model,
        GRBsetdblattrelement(m_model, GRB_DBL_ATTR_RHS, index, gurobi_numeric(rhs))
    )

    GUROBI_CATCH(
        m_model,
        GRBsetcharattrelement(m_model, GRB_CHAR_ATTR_SENSE, index, gurobi_ctr_type(type))
    )

}

void idol::Optimizers::Gurobi::hook_update_objective() {

    const auto& model = parent();
    const auto& objective = model.get_obj_expr();
    const auto sense = gurobi_obj_sense(model.get_obj_sense());

    // Linear part
    const auto& linear_terms = objective.affine().linear();
    std::vector<int> linear_indices;
    std::vector<double> linear_values;
    linear_indices.reserve(linear_terms.size());
    linear_values.reserve(linear_terms.size());

    for (const auto& [var, coeff] : linear_terms) {
        linear_indices.push_back(lazy(var).impl());
        linear_values.push_back(gurobi_numeric(coeff));
    }

    // Constant
    double linear_constant = gurobi_numeric(objective.affine().constant());

    // Quadratic part
    std::vector<int> quadratic_indices1, quadratic_indices2;
    std::vector<double> quadratic_values;
    quadratic_indices1.reserve(objective.size());
    quadratic_indices2.reserve(objective.size());
    quadratic_values.reserve(objective.size());

    for (const auto& [pair, coeff] : objective) {
        quadratic_indices1.push_back(lazy(pair.first).impl());
        quadratic_indices2.push_back(lazy(pair.second).impl());
        quadratic_values.push_back(gurobi_numeric(coeff));
    }

    GUROBI_CATCH(
                m_model,
                GRBsetobjective(m_model,
                              sense,
                              linear_constant,
                              (int) linear_indices.size(),
                              linear_indices.data(),
                              linear_values.data(),
                              quadratic_indices1.size(),
                              quadratic_indices1.data(),
                              quadratic_indices2.data(),
                              quadratic_values.data()
                              )
            )

}

void idol::Optimizers::Gurobi::hook_update_rhs() {

    const auto& model = parent();

    for (const auto& ctr : model.ctrs()) {
        int constr_index = lazy(ctr).impl();
        double rhs = gurobi_numeric(model.get_ctr_rhs(ctr));

        GUROBI_CATCH(
            m_model,
            GRBsetdblattrelement(m_model, "RHS", constr_index, rhs)
        )
    }

}

void idol::Optimizers::Gurobi::hook_remove(const Var& t_var) {

    const auto& impl = lazy(t_var).impl();
    throw Exception("Not implemented."); // TODO
    //m_model.remove(impl);

}

void idol::Optimizers::Gurobi::hook_remove(const Ctr& t_ctr) {

    const auto& impl = lazy(t_ctr).impl();
    throw Exception("Not implemented."); // TODO
    //m_model.remove(impl);

}

void idol::Optimizers::Gurobi::hook_optimize() {
    set_solution_index(0);

    GUROBI_CATCH(
        m_model,
        GRBoptimize(m_model)
    )

}

void idol::Optimizers::Gurobi::hook_write(const std::string &t_name) {
    GUROBI_CATCH(
        m_model,
        GRBwrite(m_model, t_name.c_str())
    )
}

void idol::Optimizers::Gurobi::hook_update_objective_sense() {
    GUROBI_CATCH(
        m_model,
        GRBsetintattr(
            m_model,
            "ModelSense",
            gurobi_obj_sense(parent().get_obj_sense())
        )
    )
}

void idol::Optimizers::Gurobi::hook_update_matrix(const Ctr &t_ctr, const Var &t_var, double t_constant) {

    const auto var_impl = lazy(t_var).impl();
    const auto ctr_impl = lazy(t_ctr).impl();

    GUROBI_CATCH(
        m_model,
        GRBchgcoeffs(
            m_model,
            1,
            (int*) &ctr_impl,
            (int*) &var_impl,
            &t_constant
        )
    )

}

void idol::Optimizers::Gurobi::hook_update() {
    GUROBI_CATCH(
        m_model,
        GRBupdatemodel(m_model)
    )
}

void idol::Optimizers::Gurobi::set_param_time_limit(double t_time_limit) {
    GUROBI_CATCH(
        m_model,
        GRBsetdblparam(GRBgetenv(m_model), "TimeLimit", t_time_limit)
    )
    Optimizer::set_param_time_limit(t_time_limit);
}

void idol::Optimizers::Gurobi::set_param_threads(unsigned int t_thread_limit) {
    GUROBI_CATCH(
        m_model,
        GRBsetintparam(GRBgetenv(m_model), "Threads", (int) t_thread_limit)
    )
    Optimizer::set_param_threads(t_thread_limit);
}

void idol::Optimizers::Gurobi::set_param_best_obj_stop(double t_best_obj_stop) {
    GUROBI_CATCH(
        m_model,
        GRBsetdblparam(GRBgetenv(m_model), "BestObjStop", t_best_obj_stop)
    )
    Optimizer::set_param_best_obj_stop(t_best_obj_stop);
}

void idol::Optimizers::Gurobi::set_param_best_bound_stop(double t_best_bound_stop) {
    GUROBI_CATCH(
        m_model,
        GRBsetdblparam(GRBgetenv(m_model), "BestBdStop", t_best_bound_stop)
    )
    Optimizer::set_param_best_bound_stop(t_best_bound_stop);
}

void idol::Optimizers::Gurobi::set_param_presolve(bool t_value) {
    GUROBI_CATCH(
        m_model,
        GRBsetintparam(GRBgetenv(m_model), "Presolve", t_value)
    )
    Optimizer::set_param_presolve(t_value);
}

void idol::Optimizers::Gurobi::set_param_infeasible_or_unbounded_info(bool t_value) {
    GUROBI_CATCH(
        m_model,
        GRBsetintparam(GRBgetenv(m_model), "InfUnbdInfo", t_value)
    )
    Optimizer::set_param_infeasible_or_unbounded_info(t_value);
}

void idol::Optimizers::Gurobi::add_callback(Callback *t_ptr_to_callback) {

    throw Exception("Not implemented."); // TODO

    /*
    if (!m_gurobi_callback) {
        m_gurobi_callback = std::make_unique<GurobiCallbackI>(*this);
        m_model.setCallback(m_gurobi_callback.get());
    }

    m_gurobi_callback->add_callback(t_ptr_to_callback);
    */
}

void idol::Optimizers::Gurobi::set_lazy_cut(bool t_value) {
    GUROBI_CATCH(
        m_model,
        GRBsetintparam(GRBgetenv(m_model), GRB_INT_PAR_LAZYCONSTRAINTS, t_value)
    )
}

idol::SolutionStatus idol::Optimizers::Gurobi::get_status() const {
    int status = 0;
    GUROBI_CATCH(
        m_model,
        GRBgetintattr(m_model, "Status", &status)
    )

    return gurobi_status(status).first;
}

idol::SolutionReason idol::Optimizers::Gurobi::get_reason() const {
    int status = 0;
    GUROBI_CATCH(
        m_model,
        GRBgetintattr(m_model, "Status", &status)
    )

    return gurobi_status(status).second;
}

double idol::Optimizers::Gurobi::get_best_obj() const {

    const auto status = get_status();

    if (status == Unbounded) {
        return -Inf;
    }

    if (status == Infeasible || status == InfOrUnbnd) {
        return +Inf;
    }

    int solution_index = 0;
    GUROBI_CATCH(
        m_model,
        GRBgetintparam(GRBgetenv(m_model), "SolutionNumber", &solution_index)
    )

    if (solution_index == 0) {
        double result = 0.0;
        GUROBI_CATCH(
            m_model,
            GRBgetdblattr(m_model, "ObjVal", &result)
        )
        return result;
    }

    double result = 0.0;
    GUROBI_CATCH(
        m_model,
        GRBgetdblattr(m_model, "PoolObjVal", &result)
    )
    return result;

}

double idol::Optimizers::Gurobi::get_best_bound() const {

    const auto status = get_status();

    if (status == Unbounded) {
        return -Inf;
    }

    if (status == Infeasible) {
        return +Inf;
    }

    int solution_index = 0;
    GUROBI_CATCH(
        m_model,
        GRBgetintparam(GRBgetenv(m_model), "SolutionNumber", &solution_index)
    )

    if (solution_index == 0) {
        double result = 0.0;
        GUROBI_CATCH(
            m_model,
            GRBgetdblattr(m_model, "ObjBound", &result)
        )
        return result;
    }

    double result = 0.0;
    GUROBI_CATCH(
        m_model,
        GRBgetdblattr(m_model, "PoolObjBound", &result)
    )
    return result;

}

double idol::Optimizers::Gurobi::get_var_primal(const Var &t_var) const {

    if (const auto status = get_status() ; status != Optimal && status != Feasible && status != SubOptimal) {
        throw Exception("Primal solution not available.");
    }

    auto index = lazy(t_var).impl();
    double result = 0.0;

    int solution_index = 0;
    GUROBI_CATCH(
        m_model,
        GRBgetintparam(GRBgetenv(m_model), "SolutionNumber", &solution_index)
    )

    if (solution_index == 0) {
        GUROBI_CATCH(
            m_model,
            GRBgetdblattrarray(m_model, "X", index, 1, &result)
        )
        return result;
    }

    GUROBI_CATCH(
        m_model,
        GRBgetdblattrarray(m_model, "Xn", index, 1, &result)
    )
}

double idol::Optimizers::Gurobi::get_var_ray(const Var &t_var) const {

    if (const auto status = get_status() ; status != Unbounded) {
        throw Exception("Ray not available.");
    }

    int index = lazy(t_var).impl();
    double result = 0.0;

    GUROBI_CATCH(
        m_model,
        GRBgetdblattrelement(m_model, "UnbdRay", index, &result)
    )

    return result;
}

double idol::Optimizers::Gurobi::get_ctr_dual(const Ctr &t_ctr) const {

    if (const auto status = get_status() ; status != Optimal && status != Feasible && status != SubOptimal) {
        throw Exception("Dual solution not available.");
    }

    int index = lazy(t_ctr).impl();
    double result = 0.0;

    GUROBI_CATCH(
        m_model,
        GRBgetdblattrelement(m_model, "Pi", index, &result)
    )

    return result;
}

double idol::Optimizers::Gurobi::get_ctr_farkas(const Ctr &t_ctr) const {

    if (const auto status = get_status() ; status != Infeasible) {
        throw Exception("Farkas solution not available.");
    }

    int index = lazy(t_ctr).impl();
    double result = 0.0;

    GUROBI_CATCH(
        m_model,
        GRBgetdblattrelement(m_model, "FarkasDual", index, &result)
    )

    return -result;
}

double idol::Optimizers::Gurobi::get_relative_gap() const {
    throw Exception("Not implemented");
}

double idol::Optimizers::Gurobi::get_absolute_gap() const {
    throw Exception("Not implemented");
}

unsigned int idol::Optimizers::Gurobi::get_n_solutions() const {

    if (const auto status = get_status() ; status == Unbounded || status == Infeasible || status == InfOrUnbnd) {
        return 0;
    }

    int solution_index = 0;

    GUROBI_CATCH(
        m_model,
        GRBgetintattr(m_model, "SolCount", &solution_index)
    )

    return solution_index;
}

unsigned int idol::Optimizers::Gurobi::get_solution_index() const {
    int result = 0;

    GUROBI_CATCH(
        m_model,
        GRBgetintparam(GRBgetenv(m_model), "SolutionNumber", &result)
    )

    return result;
}

void idol::Optimizers::Gurobi::set_solution_index(unsigned int t_index) {
    GUROBI_CATCH(
        m_model,
        GRBsetintparam(GRBgetenv(m_model), "SolutionNumber", (int) t_index)
    )
}

void idol::Optimizers::Gurobi::set_max_n_solution_in_pool(unsigned int t_value) {
    int value = std::min<int>(2000000000, (int) t_value);
    GUROBI_CATCH(
        m_model,
        GRBsetintparam(GRBgetenv(m_model), GRB_INT_PAR_POOLSOLUTIONS, value)
    )
}

void idol::Optimizers::Gurobi::set_param_logs(bool t_value) {
    GUROBI_CATCH(
        m_model,
        GRBsetintparam(GRBgetenv(m_model), "OutputFlag", t_value ? 1 : 0)
    )
    Optimizer::set_param_logs(t_value);
}

void idol::Optimizers::Gurobi::set_param(const std::string& t_param, int t_value) {
    GUROBI_CATCH(
        m_model,
        GRBsetintparam(GRBgetenv(m_model), t_param.c_str(), t_value)
    )
}

void idol::Optimizers::Gurobi::set_param(const std::string& t_param, double t_value) {
    GUROBI_CATCH(
        m_model,
        GRBsetintparam(GRBgetenv(m_model), t_param.c_str(), t_value)
    )
}

void idol::Optimizers::Gurobi::set_tol_mip_relative_gap(double t_relative_gap_tolerance) {
    Optimizer::set_tol_mip_relative_gap(t_relative_gap_tolerance);
    GUROBI_CATCH(
        m_model,
        GRBsetdblparam(GRBgetenv(m_model), "MIPGap", t_relative_gap_tolerance)
    )
}

void idol::Optimizers::Gurobi::set_tol_mip_absolute_gap(double t_absolute_gap_tolerance) {
    Optimizer::set_tol_mip_absolute_gap(t_absolute_gap_tolerance);
    GUROBI_CATCH(
        m_model,
        GRBsetdblparam(GRBgetenv(m_model), "MIPGapAbs", t_absolute_gap_tolerance)
    )
}

void idol::Optimizers::Gurobi::set_tol_feasibility(double t_tol_feasibility) {
    Optimizer::set_tol_feasibility(t_tol_feasibility);
    GUROBI_CATCH(
        m_model,
        GRBsetdblparam(GRBgetenv(m_model), "FeasibilityTol", t_tol_feasibility)
    )
}

void idol::Optimizers::Gurobi::set_tol_optimality(double t_tol_optimality) {
    Optimizer::set_tol_optimality(t_tol_optimality);
    GUROBI_CATCH(
        m_model,
        GRBsetdblparam(GRBgetenv(m_model), "OptimalityTol", t_tol_optimality)
    )
}

void idol::Optimizers::Gurobi::set_tol_integer(double t_tol_integer) {
    Optimizer::set_tol_integer(t_tol_integer);
    GUROBI_CATCH(
        m_model,
        GRBsetdblparam(GRBgetenv(m_model), "IntFeasTol", t_tol_integer)
    )
}

/*
idol::Model idol::Optimizers::Gurobi::read_from_file(idol::Env *t_env, const std::string &t_filename) {

    Model result(t_env);

    std::unique_ptr<GRBModel> model;
    GUROBI_CATCH(model = std::make_unique<GRBModel>(get_global_env(), t_filename);)

    const unsigned int n_vars = model->get(GRB_IntAttr_NumVars);
    const unsigned int n_ctrs = model->get(GRB_IntAttr_NumConstrs);
    const unsigned int n_quad_ctrs = model->get(GRB_IntAttr_NumQConstrs);

    result.reserve_vars(n_vars);
    for (unsigned int j = 0 ; j < n_vars ; ++j) {

        const auto& var = model->getVar(j);
        const double lb = var.get(GRB_DoubleAttr_LB);
        const double ub = var.get(GRB_DoubleAttr_UB);
        const double obj = var.get(GRB_DoubleAttr_Obj);
        VarType type = idol_var_type(var.get(GRB_CharAttr_VType));
        const auto name = var.get(GRB_StringAttr_VarName);

        result.add_var(lb, ub, type, obj, name);
    }

    const auto parse_linear = [&](const GRBLinExpr& t_lin_expr) {

        AffExpr<Var> result_ = t_lin_expr.getConstant();
        auto& linear_ = result_.linear();
        const auto n_terms = t_lin_expr.size();
        linear_.reserve(n_terms);

        for (unsigned int j = 0 ; j < n_terms ; ++j) {
            auto var = t_lin_expr.getVar(j);
            linear_.set(result.get_var_by_index(var.index()), t_lin_expr.getCoeff(j));
        }

        return result_;
    };

    // This was commented out
    const auto parse_quadratic = [&](const GRBQuadExpr& t_quad_expr) {
        AffExpr result_ = parse_linear(t_quad_expr.getLinExpr());

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

    result.reserve_ctrs(n_ctrs);
    for (unsigned int i = 0 ; i < n_ctrs ; ++i) {

        const auto& ctr = model->getConstr(i);
        const auto& expr = model->getRow(ctr);
        const double rhs = ctr.get(GRB_DoubleAttr_RHS);
        const auto type = idol_ctr_type(ctr.get(GRB_CharAttr_Sense));
        const auto& name = ctr.get(GRB_StringAttr_ConstrName);

        AffExpr lhs = parse_linear(expr);
        add_ctr(lhs, rhs, type, name);
    }

    // This was commented out
    for (unsigned int i = 0 ; i < n_quad_ctrs ; ++i) {

        const auto& original_ctr = model->getQConstrs()[i];
        const auto& expr = model->getQCRow(original_ctr);
        const double rhs = original_ctr.get(GRB_DoubleAttr_QCRHS);
        const auto type = idol_ctr_type(original_ctr.get(GRB_CharAttr_QCSense));
        const auto& name = original_ctr.get(GRB_StringAttr_QCName);

        AffExpr lhs = parse_quadratic(expr);
        add_ctr(row, rhs, type, name);

    }

    const auto sense = model->get(GRB_IntAttr_ModelSense);
    result.set_obj_sense(idol_obj_sense(sense));

    const auto& objective = model->getObjective();
    result.set_obj_expr(parse_linear(objective.getLinExpr()));

    return std::move(result);
}
*/

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
    const double constant = parent().get_obj_expr().affine().constant();
    GUROBI_CATCH(
        m_model,
        GRBsetdblattr(m_model, "ObjCon", constant)
    )
}

double idol::Optimizers::Gurobi::get_var_reduced_cost(const idol::Var &t_var) const {

    if (const auto status = get_status() ; status != Optimal && status != Feasible && status != SubOptimal) {
        throw Exception("Reduced cost not available.");
    }

    int index = lazy(t_var).impl(); // variable index in C API
    double result = 0.0;

    GUROBI_CATCH(
        m_model,
        GRBgetdblattrelement(m_model, GRB_DBL_ATTR_RC, index, &result)
    )

    return result;
}

void idol::Optimizers::Gurobi::hook_remove(const idol::QCtr &t_ctr) {
    throw Exception("Not implemented."); // TODO
    //m_model.remove(lazy(t_ctr).impl());
}

int idol::Optimizers::Gurobi::hook_add(const idol::SOSCtr &t_ctr) {

    const auto& model = parent();
    const auto src_vars = model.get_sosctr_vars(t_ctr);
    const auto n = src_vars.size();

    std::vector<int> indices(n);
    for (int i = 0; i < n; ++i) {
        indices[i] = lazy(src_vars[i]).impl();
    }

    auto* weights = model.get_sosctr_weights(t_ctr).data();
    const auto type = model.is_sos1(t_ctr) ? 1 : 2;
    int beginning_index = 0;

    const unsigned int index = model.get_sosctr_index(t_ctr);

    GUROBI_CATCH(
        m_model,
        GRBaddsos(
            m_model,
            1,
            n,
            (int*) &type,
            &beginning_index,
            indices.data(),
            (double*) weights
        )
    )

    return index;
}

void idol::Optimizers::Gurobi::hook_remove(const idol::SOSCtr &t_ctr) {
    throw Exception("Not implemented."); // TODO
    // m_model.remove(lazy(t_ctr).impl());
}

#endif