//
// Created by henri on 07.04.25.
//
#ifdef IDOL_USE_CPLEX

#include "idol/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

std::unique_ptr<IloEnv> idol::Optimizers::Cplex::s_global_env;

IloEnv &idol::Optimizers::Cplex::get_global_env() {
    if (!s_global_env) {
        s_global_env = std::make_unique<IloEnv>();
    }
    return *s_global_env;
}

IloNumVar::Type idol::Optimizers::Cplex::cplex_var_type(int t_type) {

    if (m_continuous_relaxation) {
        return ILOFLOAT;
    }

    switch (t_type) {
        case Continuous: return ILOFLOAT;
        case Integer: return ILOINT;
        case Binary: return ILOBOOL;
        default:;
    }
    throw Exception("Unsupported variable type: " + std::to_string(t_type));
}

double idol::Optimizers::Cplex::cplex_numeric(double t_value) {
    if (is_pos_inf(t_value)) { return IloInfinity; }
    if (is_neg_inf(t_value)) { return -IloInfinity; }
    return t_value;
}

idol::Optimizers::Cplex::Cplex(const Model &t_model, bool t_continuous_relaxation, IloEnv &t_env)
        : OptimizerWithLazyUpdates(t_model),
          m_continuous_relaxation(t_continuous_relaxation),
          m_env(t_env),
          m_model(t_env),
          m_cplex(m_model) {

    // Parameters
    m_cplex.setParam(IloCplex::Param::MIP::Display, get_param_logs());
    m_cplex.setParam(IloCplex::Param::MIP::Limits::LowerObjStop, get_param_best_bound_stop());
    m_cplex.setParam(IloCplex::Param::MIP::Tolerances::UpperCutoff, get_param_best_obj_stop());
    m_cplex.setParam(IloCplex::Param::TimeLimit, get_param_time_limit());
    m_cplex.setParam(IloCplex::Param::Preprocessing::Presolve, get_param_presolve());
    m_cplex.setParam(IloCplex::Param::Threads, (int) get_param_thread_limit());
    m_cplex.setParam(IloCplex::Param::MIP::Limits::RepairTries, get_param_infeasible_or_unbounded_info());

    // Tolerances
    m_cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, get_tol_mip_relative_gap());
    m_cplex.setParam(IloCplex::Param::MIP::Tolerances::AbsMIPGap, get_tol_mip_absolute_gap());
    m_cplex.setParam(IloCplex::Param::MIP::Tolerances::Integrality, get_tol_integer());
    m_cplex.setParam(IloCplex::Param::Simplex::Tolerances::Feasibility, get_tol_feasibility());
    m_cplex.setParam(IloCplex::Param::Simplex::Tolerances::Optimality, get_tol_optimality());

}

void idol::Optimizers::Cplex::hook_build() {

    const auto& model = parent();
    const auto& objective = model.get_obj_expr();

    if (!objective.has_quadratic()) {
        hook_update_objective_sense();
        update_objective_constant();
        set_objective_as_updated();
    }

    set_rhs_as_updated();

}

IloNumVar idol::Optimizers::Cplex::hook_add(const Var& t_var, bool t_add_column) {

    const auto& model = parent();
    const auto& column = model.get_var_column(t_var);
    const auto lb = model.get_var_lb(t_var);
    const auto ub = model.get_var_ub(t_var);
    const auto objective = model.get_var_obj(t_var);
    const auto type = cplex_var_type(model.get_var_type(t_var));
    const auto& name = t_var.name();

    IloNumColumn col(m_env);
    if (t_add_column) {

        for (const auto& [ctr, constant] : column) {

            if (!has_lazy(ctr)) { // if the constraint has no lazy, it will be created right after
                continue;
            }

            auto& impl = lazy(ctr).impl();

            col += impl(constant);

        }

    }

    IloNumVar var(col, lb, ub, type, name.c_str());
    m_model.add(var);
    return var;
}

IloRange idol::Optimizers::Cplex::hook_add(const Ctr& t_ctr) {

    const auto& model = parent();
    const auto& row = model.get_ctr_row(t_ctr);
    const auto type = model.get_ctr_type(t_ctr);
    const auto rhs = model.get_ctr_rhs(t_ctr);
    const auto& name = t_ctr.name();

    double lb = -IloInfinity, ub = IloInfinity;
    switch (type) {
        case Equal:
            lb = rhs;
            ub = rhs;
            break;
        case LessOrEqual:
            lb = -IloInfinity;
            ub = rhs;
            break;
        case GreaterOrEqual:
            lb = rhs;
            ub = IloInfinity;
            break;
        default:
            throw Exception("Unsupported constraint type: " + std::to_string(type));
    }

    IloExpr expr(m_env);
    const unsigned int n = row.size();
    for (const auto &[var, constant]: row) {
        expr += cplex_numeric(constant) * lazy(var).impl();
    }

    IloRange ctr(m_env, lb, expr, ub, name.c_str());
    m_model.add(ctr);
    return ctr;
}

IloRange idol::Optimizers::Cplex::hook_add(const idol::QCtr &t_ctr) {

    const auto& model = parent();
    const auto& expr = model.get_qctr_expr(t_ctr);
    const auto type = model.get_qctr_type(t_ctr);
    const auto& name = t_ctr.name();

    double lb = -IloInfinity, ub = IloInfinity;
    switch (type) {
        case Equal:
            lb = 0;
            ub = 0;
            break;
        case LessOrEqual:
            lb = -IloInfinity;
            ub = 0;
            break;
        case GreaterOrEqual:
            lb = 0;
            ub = IloInfinity;
            break;
        default:
            throw Exception("Unsupported constraint type: " + std::to_string(type));
    }

    IloExpr quad_expr(m_env);
    quad_expr += expr.affine().constant();

    for (const auto& [var, constant]: expr.affine().linear()) {
        quad_expr += cplex_numeric(constant) * lazy(var).impl();
    }

    for (const auto& [pair, constant]: expr) {
        quad_expr += constant * lazy(pair.first).impl() * lazy(pair.second).impl();
    }

    IloRange ctr(m_env, lb, quad_expr, ub, name.c_str());
    m_model.add(ctr);
    return ctr;
}

void idol::Optimizers::Cplex::hook_update(const Var& t_var) {

    const auto& model = parent();
    auto& impl = lazy(t_var).impl();
    const double lb = model.get_var_lb(t_var);
    const double ub = model.get_var_ub(t_var);
    const int type = model.get_var_type(t_var);
    const double obj = model.get_var_obj(t_var);

    impl.setBounds(lb, ub);
    // m_model.add(IloConversion(m_env, impl, cplex_var_type(type)));
    std::cerr << "WARNING: conversion is not implemented" << std::endl;
    // impl.set(GRB_DoubleAttr_Obj, gurobi_numeric(obj));
    std::cerr << "WARNING: updating a single coefficient in objective" << std::endl;

}

void idol::Optimizers::Cplex::hook_update(const Ctr& t_ctr) {

    const auto& model = parent();
    auto& impl = lazy(t_ctr).impl();

    const auto& rhs = model.get_ctr_rhs(t_ctr);
    const auto type = model.get_ctr_type(t_ctr);

    double lb = -IloInfinity, ub = IloInfinity;
    switch (type) {
        case Equal:
            lb = rhs;
            ub = rhs;
            break;
        case LessOrEqual:
            lb = -IloInfinity;
            ub = rhs;
            break;
        case GreaterOrEqual:
            lb = rhs;
            ub = IloInfinity;
            break;
        default:
            throw Exception("Unsupported constraint type: " + std::to_string(type));
    }

    impl.setBounds(lb, ub);

}

void idol::Optimizers::Cplex::hook_update_objective() {

    throw Exception("Not implemented");

    /*
    const auto& model = parent();
    const auto& objective = model.get_obj_expr();
    const auto sense = gurobi_obj_sense(model.get_obj_sense());

    GRBLinExpr linear_expr = gurobi_numeric(objective.affine().constant());

    const unsigned int n = objective.affine().linear().size();
    auto* vars = new GRBVar[n];
    auto* vals = new double[n];
    unsigned int i = 0;
    for (const auto& [var, constant] : objective.affine().linear()) {
        vars[i] = lazy(var).impl();
        vals[i] = gurobi_numeric(constant);
        ++i;
    }
    linear_expr.addTerms(vals, vars, (int) n);
    delete[] vars;
    delete[] vals;

    if (!objective.has_quadratic()) {
        m_model.setObjective(linear_expr, sense);
        return;
    }

    GRBQuadExpr quad_expr(linear_expr);
    for (const auto& [pair, constant] : objective) {
        quad_expr += gurobi_numeric(constant) * lazy(pair.first).impl() * lazy(pair.second).impl();
    }

    m_model.setObjective(quad_expr, sense);
    */
}

void idol::Optimizers::Cplex::hook_update_rhs() {

    const auto& model = parent();

    for (const auto& ctr : model.ctrs()) {
        hook_update(ctr);
    }

}

void idol::Optimizers::Cplex::hook_remove(const Var& t_var) {

    const auto& impl = lazy(t_var).impl();
    m_model.remove(impl);

}

void idol::Optimizers::Cplex::hook_remove(const Ctr& t_ctr) {

    const auto& impl = lazy(t_ctr).impl();
    m_model.remove(impl);

}

void idol::Optimizers::Cplex::hook_optimize() {
    set_solution_index(0);
    m_cplex.solve();
}

void idol::Optimizers::Cplex::hook_write(const std::string &t_name) {
    throw Exception("Not implemented");
}

void idol::Optimizers::Cplex::hook_update_objective_sense() {
    throw Exception("Not implemented");
}

void idol::Optimizers::Cplex::hook_update_matrix(const Ctr &t_ctr, const Var &t_var, double t_constant) {

    const auto& var_impl = lazy(t_var).impl();
    const auto& ctr_impl = lazy(t_ctr).impl();

    throw Exception("Not implemented");

}

void idol::Optimizers::Cplex::hook_update() {

}

void idol::Optimizers::Cplex::set_param_time_limit(double t_time_limit) {
    m_cplex.setParam(IloCplex::Param::TimeLimit, t_time_limit);
    Optimizer::set_param_time_limit(t_time_limit);
}

void idol::Optimizers::Cplex::set_param_threads(unsigned int t_thread_limit) {
    m_cplex.setParam(IloCplex::Param::Threads, (int) t_thread_limit);
    Optimizer::set_param_threads(t_thread_limit);
}

void idol::Optimizers::Cplex::set_param_best_obj_stop(double t_best_obj_stop) {
    m_cplex.setParam(IloCplex::Param::MIP::Limits::LowerObjStop, t_best_obj_stop);
    Optimizer::set_param_best_obj_stop(t_best_obj_stop);
}

void idol::Optimizers::Cplex::set_param_best_bound_stop(double t_best_bound_stop) {
    m_cplex.setParam(IloCplex::Param::MIP::Limits::UpperObjStop, t_best_bound_stop);
    Optimizer::set_param_best_bound_stop(t_best_bound_stop);
}

void idol::Optimizers::Cplex::set_param_presolve(bool t_value) {
    m_cplex.setParam(IloCplex::Param::Preprocessing::Presolve, t_value);
    Optimizer::set_param_presolve(t_value);
}

void idol::Optimizers::Cplex::set_param_infeasible_or_unbounded_info(bool t_value) {
    throw Exception("Not implemented");
    Optimizer::set_param_infeasible_or_unbounded_info(t_value);
}

void idol::Optimizers::Cplex::add_callback(Callback *t_ptr_to_callback) {
    throw Exception("Not implemented");
}

idol::SolutionStatus idol::Optimizers::Cplex::get_status() const {
    throw Exception("Not implemented");
}

idol::SolutionReason idol::Optimizers::Cplex::get_reason() const {
    throw Exception("Not implemented");
}

double idol::Optimizers::Cplex::get_best_obj() const {
    throw Exception("Not implemented");
}

double idol::Optimizers::Cplex::get_best_bound() const {
    throw Exception("Not implemented");
}

double idol::Optimizers::Cplex::get_var_primal(const Var &t_var) const {
    throw Exception("Not implemented");
}

double idol::Optimizers::Cplex::get_var_ray(const Var &t_var) const {
    throw Exception("Not implemented");
}

double idol::Optimizers::Cplex::get_ctr_dual(const Ctr &t_ctr) const {
    throw Exception("Not implemented");
}

double idol::Optimizers::Cplex::get_ctr_farkas(const Ctr &t_ctr) const {
    throw Exception("Not implemented");
}

double idol::Optimizers::Cplex::get_relative_gap() const {
    throw Exception("Not implemented");
}

double idol::Optimizers::Cplex::get_absolute_gap() const {
    throw Exception("Not implemented");
}

unsigned int idol::Optimizers::Cplex::get_n_solutions() const {

    if (const auto status = get_status() ; status == Unbounded || status == Infeasible || status == InfOrUnbnd) {
        return 0;
    }

    throw Exception("Not implemented");
}

unsigned int idol::Optimizers::Cplex::get_solution_index() const {
    throw Exception("Not implemented");
}

void idol::Optimizers::Cplex::set_solution_index(unsigned int t_index) {
    throw Exception("Not implemented");
}

void idol::Optimizers::Cplex::set_max_n_solution_in_pool(unsigned int t_value) {
    throw Exception("Not implemented");
}

void idol::Optimizers::Cplex::set_param_logs(bool t_value) {
    m_cplex.setParam(IloCplex::Param::MIP::Display, (int) t_value);
    Optimizer::set_param_logs(t_value);
}

void idol::Optimizers::Cplex::set_tol_mip_relative_gap(double t_relative_gap_tolerance) {
    Optimizer::set_tol_mip_relative_gap(t_relative_gap_tolerance);
    m_cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, t_relative_gap_tolerance);
}

void idol::Optimizers::Cplex::set_tol_mip_absolute_gap(double t_absolute_gap_tolerance) {
    Optimizer::set_tol_mip_absolute_gap(t_absolute_gap_tolerance);
    m_cplex.setParam(IloCplex::Param::MIP::Tolerances::AbsMIPGap, t_absolute_gap_tolerance);
}

void idol::Optimizers::Cplex::set_tol_feasibility(double t_tol_feasibility) {
    Optimizer::set_tol_feasibility(t_tol_feasibility);
    m_cplex.setParam(IloCplex::Param::Simplex::Tolerances::Feasibility, t_tol_feasibility);
}

void idol::Optimizers::Cplex::set_tol_optimality(double t_tol_optimality) {
    Optimizer::set_tol_optimality(t_tol_optimality);
    m_cplex.setParam(IloCplex::Param::Simplex::Tolerances::Optimality, t_tol_optimality);
}

void idol::Optimizers::Cplex::set_tol_integer(double t_tol_integer) {
    Optimizer::set_tol_integer(t_tol_integer);
    m_cplex.setParam(IloCplex::Param::MIP::Tolerances::Integrality, t_tol_integer);
}

idol::Model idol::Optimizers::Cplex::read_from_file(idol::Env &t_env, const std::string &t_filename) {
    throw Exception("Not implemented");
}

void idol::Optimizers::Cplex::update_objective_constant() {
    const double constant = parent().get_obj_expr().affine().constant();
    throw Exception("Not implemented");
}

double idol::Optimizers::Cplex::get_var_reduced_cost(const idol::Var &t_var) const {

    if (const auto status = get_status() ; status != Optimal && status != Feasible && status != SubOptimal) {
        throw Exception("Reduced cost not available.");
    }

    throw Exception("Not implemented");
}

void idol::Optimizers::Cplex::hook_remove(const idol::QCtr &t_ctr) {
    m_model.remove(lazy(t_ctr).impl());
}

#endif