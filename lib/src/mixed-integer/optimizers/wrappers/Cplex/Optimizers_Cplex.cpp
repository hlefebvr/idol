//
// Created by henri on 07.04.25.
//
#ifdef IDOL_USE_CPLEX

#include "idol/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/modeling/constraints/TempQCtr.h"

#define CATCH_CPLEX(cmd) \
try { \
    cmd \
} catch (IloException& error) { \
    throw Exception("Cplex exception: " + std::string(error.getMessage()) ); \
}

IloNumVar::Type idol::Optimizers::Cplex::cplex_var_type(int t_type) {

    if (m_continuous_relaxation) {
        return IloNumVar::Type::Float;
    }

    switch (t_type) {
        case Continuous: return IloNumVar::Type::Float;
        case Integer: return IloNumVar::Type::Int;
        case Binary: return IloNumVar::Type::Bool;
        default:;
    }
    throw Exception("Unsupported variable type: " + std::to_string(t_type));
}

double idol::Optimizers::Cplex::cplex_numeric(double t_value) {
    if (is_pos_inf(t_value)) { return IloGetInfinity(); }
    if (is_neg_inf(t_value)) { return -IloGetInfinity(); }
    return t_value;
}

idol::Optimizers::Cplex::Cplex(const Model &t_model, bool t_continuous_relaxation)
        : OptimizerWithLazyUpdates(t_model),
          m_continuous_relaxation(t_continuous_relaxation),
          m_model(m_env),
          m_objective(m_env),
          m_cplex(m_model) {

    // Parameters
    m_cplex.setParam(IloCplex::Param::MIP::Display, ((int) get_param_logs()) * 4);
    m_cplex.setParam(IloCplex::Param::MIP::Limits::LowerObjStop, get_param_best_obj_stop());
    m_cplex.setParam(IloCplex::Param::MIP::Limits::UpperObjStop, get_param_best_bound_stop());
    m_cplex.setParam(IloCplex::Param::TimeLimit, std::min(1e+75, get_param_time_limit()));
    m_cplex.setParam(IloCplex::Param::Preprocessing::Presolve, get_param_presolve());
    m_cplex.setParam(IloCplex::Param::Threads, (int) get_param_thread_limit());
    m_cplex.setParam(IloCplex::Param::MIP::Limits::RepairTries, get_param_infeasible_or_unbounded_info());

    // Tolerances
    m_cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, get_tol_mip_relative_gap());
    m_cplex.setParam(IloCplex::Param::MIP::Tolerances::AbsMIPGap, get_tol_mip_absolute_gap());
    m_cplex.setParam(IloCplex::Param::MIP::Tolerances::Integrality, get_tol_integer());
    m_cplex.setParam(IloCplex::Param::Simplex::Tolerances::Feasibility, get_tol_feasibility());
    m_cplex.setParam(IloCplex::Param::Simplex::Tolerances::Optimality, get_tol_optimality());

    m_model.add(m_objective);

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
    const auto lb = cplex_numeric(model.get_var_lb(t_var));
    const auto ub = cplex_numeric(model.get_var_ub(t_var));
    const auto objective = cplex_numeric(model.get_var_obj(t_var));
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

    IloNumVar var(col,lb, ub, type, name.c_str());
    m_model.add(var);
    m_objective.setLinearCoef(var, cplex_numeric(objective));
    return var;
}

IloRange idol::Optimizers::Cplex::hook_add(const Ctr& t_ctr) {

    const auto& model = parent();
    const auto& row = model.get_ctr_row(t_ctr);
    const auto type = model.get_ctr_type(t_ctr);
    const auto rhs = cplex_numeric(model.get_ctr_rhs(t_ctr));
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

    IloNumExpr expr(m_env);
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

    IloNumExpr quad_expr(m_env);
    quad_expr += cplex_numeric(expr.affine().constant());

    for (const auto& [var, constant]: expr.affine().linear()) {
        quad_expr += cplex_numeric(constant) * lazy(var).impl();
    }

    for (const auto& [pair, constant]: expr) {
        quad_expr += cplex_numeric(constant) * lazy(pair.first).impl() * lazy(pair.second).impl();
    }

    IloRange ctr(m_env, lb, quad_expr, ub, name.c_str());
    m_model.add(ctr);
    return ctr;
}

void idol::Optimizers::Cplex::hook_update(const Var& t_var) {

    const auto& model = parent();
    auto& impl = lazy(t_var).impl();
    const double lb = cplex_numeric(model.get_var_lb(t_var));
    const double ub = cplex_numeric(model.get_var_ub(t_var));
    const int type = model.get_var_type(t_var);
    const double obj = cplex_numeric(model.get_var_obj(t_var));

    impl.setBounds(lb, ub);
    // m_model.add(IloConversion(env, impl, cplex_var_type(type)));
    std::cerr << "WARNING: conversion is not implemented" << std::endl;
    m_objective.setLinearCoef(impl, cplex_numeric(obj));

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

    const auto& model = parent();
    const auto& objective = model.get_obj_expr();
    const auto sense = model.get_obj_sense();

    IloNumExpr expr(m_env);
    expr += cplex_numeric(objective.affine().constant());

    for (const auto& [var, constant] : objective.affine().linear()) {
        expr += cplex_numeric(constant) * lazy(var).impl();
    }

    for (const auto& [pair, constant] : objective) {
        expr += cplex_numeric(constant) * lazy(pair.first).impl() * lazy(pair.second).impl();
    }

    auto cplex_objective = m_objective;
    cplex_objective.setExpr(expr);
    cplex_objective.setSense(sense == Minimize ? IloObjective::Minimize : IloObjective::Maximize);
}

void idol::Optimizers::Cplex::hook_update_rhs() {

    const auto& model = parent();

    for (const auto& ctr : model.ctrs()) {
        hook_update(ctr);
    }

}

void idol::Optimizers::Cplex::hook_remove(const Var& t_var) {

    auto impl = lazy(t_var).impl();
    m_model.remove(impl);
    impl.end();

}

void idol::Optimizers::Cplex::hook_remove(const Ctr& t_ctr) {

    auto impl = lazy(t_ctr).impl();
    m_model.remove(impl);
    impl.end();

}

void idol::Optimizers::Cplex::hook_optimize() {

    set_solution_index(0);
    CATCH_CPLEX(m_cplex.solve();)

    if (get_param_infeasible_or_unbounded_info()) {
        auto status = get_status();

        if (status == InfOrUnbnd) {
            const auto presolve = m_cplex.getParam(IloCplex::Param::Preprocessing::Presolve);
            if (presolve == true) {
                m_cplex.setParam(IloCplex::Param::Preprocessing::Presolve, false);
                m_cplex.solve();
                m_cplex.setParam(IloCplex::Param::Preprocessing::Presolve, true);
                status = get_status();
            }
        }

        if (status == Infeasible) {
            //save_farkas();
            std::cerr << "WARNING: infeasibility certificate is not implemented" << std::endl;
        } else if (status == Unbounded) {
            //save_ray();
            std::cerr << "WARNING: unbounded ray is not implemented" << std::endl;
        }
    }

}

void idol::Optimizers::Cplex::hook_write(const std::string &t_name) {
    m_cplex.exportModel(t_name.c_str());
}

void idol::Optimizers::Cplex::hook_update_objective_sense() {
    const auto sense = parent().get_obj_sense();
    m_objective.setSense(sense == Minimize ? IloObjective::Minimize : IloObjective::Maximize);
}

void idol::Optimizers::Cplex::hook_update_matrix(const Ctr &t_ctr, const Var &t_var, double t_constant) {

    const auto& var_impl = lazy(t_var).impl();
    const auto& ctr_impl = lazy(t_ctr).impl();

    throw Exception("Not implemented");

}

void idol::Optimizers::Cplex::hook_update() {

}

void idol::Optimizers::Cplex::set_param_time_limit(double t_time_limit) {
    m_cplex.setParam(IloCplex::Param::TimeLimit, std::min(t_time_limit, 1e+75));
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
    Optimizer::set_param_infeasible_or_unbounded_info(t_value);
}

void idol::Optimizers::Cplex::add_callback(Callback *t_ptr_to_callback) {
    create_callback_if_not_exists();
    m_cplex_callback->add_callback(t_ptr_to_callback);
}

void idol::Optimizers::Cplex::set_lazy_cuts(bool t_lazy_cut) {
    m_lazy_cut = t_lazy_cut;
    if (m_lazy_cut) {
        create_callback_if_not_exists();
        m_cplex.use(m_cplex_callback->create_lazy_constraint_callback());
    }
}

idol::SolutionStatus idol::Optimizers::Cplex::get_status() const {
    const auto status = m_cplex.getStatus();
    switch (status) {
        case IloAlgorithm::Unknown: return SolutionStatus::Fail;
        case IloAlgorithm::Feasible: return SolutionStatus::Feasible;
        case IloAlgorithm::Optimal: return SolutionStatus::Optimal;
        case IloAlgorithm::Infeasible: return SolutionStatus::Infeasible;
        case IloAlgorithm::Unbounded: return SolutionStatus::Unbounded;
        case IloAlgorithm::InfeasibleOrUnbounded: return SolutionStatus::InfOrUnbnd;
        case IloAlgorithm::Error: return SolutionStatus::Fail;
        // case IloAlgorithm::Bounded: return SolutionStatus::SubOptimal;
        default: throw Exception("Unknown status: " + std::to_string(status));
    }
}

idol::SolutionReason idol::Optimizers::Cplex::get_reason() const {
    const auto status = m_cplex.getCplexStatus();
    switch (status) {
        case IloCplex::Optimal: [[fallthrough]];
        case IloCplex::Unbounded: [[fallthrough]];
        case IloCplex::Infeasible: [[fallthrough]];
        case IloCplex::InfOrUnbd: return SolutionReason::Proved;
        case IloCplex::AbortObjLim: [[fallthrough]];
        case IloCplex::AbortPrimObjLim: [[fallthrough]];
        case IloCplex::AbortDualObjLim: return SolutionReason::ObjLimit;
        case IloCplex::AbortItLim: return SolutionReason::IterLimit;
        case IloCplex::AbortTimeLim: return SolutionReason::TimeLimit;
        default:
            std::cerr << "CPLEX terminated with unknown reason: " << status << std::endl;
            return SolutionReason::NotSpecified;
    }
}

double idol::Optimizers::Cplex::get_best_obj() const {
    const auto status = get_status();

    if (status == Unbounded) {
        return -Inf;
    }

    if (status == Infeasible || status == InfOrUnbnd) {
        return +Inf;
    }

    return m_cplex.getObjValue();
}

double idol::Optimizers::Cplex::get_best_bound() const {
    const auto status = get_status();

    if (status == Unbounded || status == InfOrUnbnd) {
        return -Inf;
    }

    if (status == Infeasible) {
        return +Inf;
    }

    if (!m_cplex.isMIP() && status == Optimal) {
        return get_best_obj();
    }

    return m_cplex.getBestObjValue();
}

double idol::Optimizers::Cplex::get_var_primal(const Var &t_var) const {

    if (const auto status = get_status() ; status != Optimal && status != Feasible && status != SubOptimal) {
        throw Exception("Primal solution not available.");
    }

    if (m_cplex.isMIP()) {
        CATCH_CPLEX(return m_cplex.getValue(lazy(t_var).impl(), m_solution_index);)
    }

    CATCH_CPLEX(return m_cplex.getValue(lazy(t_var).impl());)
}

double idol::Optimizers::Cplex::get_var_ray(const Var &t_var) const {
    throw Exception("Not implemented");
}

double idol::Optimizers::Cplex::get_ctr_dual(const Ctr &t_ctr) const {

    if (const auto status = get_status() ; status != Optimal && status != Feasible && status != SubOptimal) {
        throw Exception("Dual solution not available.");
    }

    CATCH_CPLEX(return m_cplex.getDual(lazy(t_ctr).impl());)
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

    if (!m_cplex.isMIP()) {
        return 1;
    }

    return m_cplex.getSolnPoolNsolns();
}

unsigned int idol::Optimizers::Cplex::get_solution_index() const {
    return m_solution_index;
}

void idol::Optimizers::Cplex::set_solution_index(unsigned int t_index) {
    m_solution_index = t_index;
}

void idol::Optimizers::Cplex::set_max_n_solution_in_pool(unsigned int t_value) {
    m_cplex.setParam(IloCplex::Param::MIP::Pool::Capacity, (CPXINT) t_value);
}

void idol::Optimizers::Cplex::set_param_logs(bool t_value) {
    m_cplex.setParam(IloCplex::Param::MIP::Display, ((int) t_value) * 4);
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

    Model result(t_env);

    IloEnv env;
    IloModel model(env);
    IloCplex cplex(env);
    IloObjective objective(env);
    IloNumVarArray vars(env);
    IloRangeArray ranges(env);
    IloRangeArray lazy(env);
    IloRangeArray cuts(env);
    cplex.importModel(model, t_filename.c_str(), objective, vars, ranges, lazy, cuts);

    const unsigned int n_vars = vars.getSize();
    const unsigned int n_ctrs = ranges.getSize();

    Map<IloInt, Var> vars_map;

    // Variables
    result.reserve_vars(n_vars);
    for (unsigned int i = 0 ; i < n_vars ; ++i) {
        const auto& var = vars[i];
        const auto cplex_type = var.getType();
        const auto lb = var.getLB();
        const auto ub = var.getUB();
        const auto name = var.getName();

        VarType type = Continuous;
        switch (cplex_type) {
            case IloNumVar::Int:
                type = Integer;
                break;
            case IloNumVar::Bool:
                type = Binary;
                break;
            default:;
        }

        const auto idol_var = result.add_var(lb, ub, type, 0, std::string(name));

        const auto [it, success] = vars_map.emplace(var.getId(), idol_var);
        assert(success);

    }

    const auto parse_expr = [&](const IloExpr& t_expr) {
        QuadExpr result;

        for (auto it = t_expr.getLinearIterator() ; it.ok() ; ++it) {
            const auto& var = it.getVar();
            const double constant = it.getCoef();
            result += constant * vars_map.at(var.getId());
        }

        for (auto it = t_expr.getQuadIterator() ; it.ok() ; ++it) {
            const auto& var1 = it.getVar1();
            const auto& var2 = it.getVar2();
            const double constant = it.getCoef();
            result += constant * vars_map.at(var1.getId()) * vars_map.at(var2.getId());
        }

        result += t_expr.getConstant();

        return result;
    };

    // Constraints
    result.reserve_ctrs(n_ctrs);
    for (unsigned int i = 0 ; i < n_ctrs ; ++i) {
        const auto& ctr = ranges[i];
        const auto& cplex_expr = ctr.getExpr();
        const auto lb = ctr.getLB();
        const double ub = ctr.getUB();
        const auto name = ctr.getName();

        auto expr = parse_expr(cplex_expr);
        CtrType type;
        double rhs;
        if (lb <= -IloInfinity) {
            type = LessOrEqual;
            rhs = ub;
        } else if (ub >= IloInfinity) {
            type = GreaterOrEqual;
            rhs = lb;
        } else {
            type = Equal;
            rhs = ub;
        }

        if (!expr.has_quadratic()) {
            result.add_ctr(TempCtr(std::move(expr.affine().linear()), type, rhs), std::string(name));
        } else {
            result.add_qctr(TempQCtr(std::move(expr - rhs), type), std::string(name));
        }
    }

    // Objective
    result.set_obj_expr(parse_expr(objective.getExpr()));

    env.end();

    return std::move(result);
}

void idol::Optimizers::Cplex::update_objective_constant() {
    const double constant = parent().get_obj_expr().affine().constant();
    m_objective.setConstant(constant);
}

double idol::Optimizers::Cplex::get_var_reduced_cost(const idol::Var &t_var) const {

    if (const auto status = get_status() ; status != Optimal && status != Feasible && status != SubOptimal) {
        throw Exception("Reduced cost not available.");
    }

    return m_cplex.getReducedCost(lazy(t_var).impl());
}

void idol::Optimizers::Cplex::hook_remove(const idol::QCtr &t_ctr) {
    m_model.remove(lazy(t_ctr).impl());
}

idol::Optimizers::Cplex::~Cplex() {
    m_model.end();
    m_cplex.end();
    m_objective.end();
}

void idol::Optimizers::Cplex::create_callback_if_not_exists() {

    if (m_cplex_callback) {
        return;
    }

    m_cplex_callback = std::make_unique<CplexCallbackI>(*this);
    m_cplex.use(m_cplex_callback->create_user_cut_callback());
    m_cplex.use(m_cplex_callback->create_branch_callback());

}

idol::CplexCallbackI &idol::Optimizers::Cplex::get_cplex_callback_interface() {
    if (!m_cplex_callback) {
        throw Exception("Cplex callback is not created");
    }
    return *m_cplex_callback;
}

std::variant<IloSOS1, IloSOS2> idol::Optimizers::Cplex::hook_add(const idol::SOSCtr &t_ctr) {

    const auto& model = parent();

    const auto& src_vars = model.get_sosctr_vars(t_ctr);
    const auto n = src_vars.size();
    IloNumVarArray vars(m_env, (int) n);
    for (unsigned int i = 0 ; i < n ; ++i) {
        vars[i] = lazy(src_vars[i]).impl();
    }

    const auto& src_weights = model.get_sosctr_weights(t_ctr);
    IloNumArray weights(m_env, (int) n);
    for (unsigned int i = 0 ; i < n ; ++i) {
        weights[i] = cplex_numeric(src_weights[i]);
    }

    if (model.is_sos1(t_ctr)) {
        auto c = IloSOS1(m_env, vars, weights, t_ctr.name().c_str());
        m_model.add(c);
        return c;
    }

    auto c = IloSOS2(m_env, vars, weights, t_ctr.name().c_str());
    m_model.add(c);
    return c;

}

void idol::Optimizers::Cplex::hook_remove(const idol::SOSCtr &t_ctr) {
    const auto& impl = lazy(t_ctr).impl();

    if (std::holds_alternative<IloSOS1>(impl)) {
        m_model.remove(std::get<IloSOS1>(impl));
    } else {
        m_model.remove(std::get<IloSOS2>(impl));
    }

}

#endif