//
// Created by henri on 10.04.25.
//
#include "idol/mixed-integer/optimizers/wrappers/Cplex/CplexCallbackI.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.h"

#ifdef IDOL_USE_CPLEX

idol::CplexCallbackI::CplexCallbackI(Optimizers::Cplex &t_parent) :
        m_parent(t_parent) {

}

void idol::CplexCallbackI::add_callback(idol::Callback *t_callback) {
    m_callbacks.emplace_back(t_callback);
}

void idol::CplexCallbackI::call(IloCplex::ControlCallbackI* t_caller, idol::CallbackEvent t_event) {

    m_caller = t_caller;

    for (auto& cb : m_callbacks) {
        execute(*cb, t_event);
    }

    m_caller = nullptr;

}

const idol::Model &idol::CplexCallbackI::original_model() const {
    return m_parent.parent();
}

void idol::CplexCallbackI::add_lazy_cut(const idol::TempCtr &t_lazy_cut) {

    if (auto* cb = dynamic_cast<Optimizers::impl::CplexLazyConstraintCallbackI*>(m_caller) ; cb) {
        auto cut = cplex_temp_constr(t_lazy_cut);
        cb->add(cut);
        return;
    }

    if (auto* cb = dynamic_cast<Optimizers::impl::CplexUserCutCallbackI*>(m_caller) ; cb) {
        if (!m_parent.lazy_cuts()) {
            throw Exception("Lazy cuts are not enabled.");
        }
        auto cut = cplex_temp_constr(t_lazy_cut);
        cb->add(cut);
        return;
    }

    throw Exception("Cannot add a lazy callback at fractional points.");

}

idol::PrimalPoint idol::CplexCallbackI::primal_solution() const {

    PrimalPoint point;
    point.set_status(Optimal);
    point.set_reason(Proved);
    point.set_objective_value(m_caller->getObjValue());
    for (const auto& var : m_parent.parent().vars()) {
        const auto& cplex_var = m_parent[var];
        point.set(var, m_caller->getValue(cplex_var));
    }

    return point;
}

const idol::Timer &idol::CplexCallbackI::time() const {
    return m_parent.time();
}

double idol::CplexCallbackI::best_obj() const {
    std::cerr << "best_obj() not implemented" << std::endl;
    throw Exception("Not implemented");
}

double idol::CplexCallbackI::best_bound() const {
    std::cerr << "best_bound() not implemented" << std::endl;
    throw Exception("Not implemented");
}

void idol::CplexCallbackI::terminate() {
    std::cerr << "terminate()" << std::endl;
    m_caller->abort();
}

idol::Optimizers::impl::CplexUserCutCallbackI *idol::CplexCallbackI::create_user_cut_callback() {
    assert(!m_user_cut_callback);
    m_user_cut_callback = std::make_unique<Optimizers::impl::CplexUserCutCallbackI>(m_parent.env(), this);
    return m_user_cut_callback.get();
}

idol::Optimizers::impl::CplexLazyConstraintCallbackI *idol::CplexCallbackI::create_lazy_constraint_callback() {
    assert(!m_lazy_constraint_callback);
    m_lazy_constraint_callback = std::make_unique<Optimizers::impl::CplexLazyConstraintCallbackI>(m_parent.env(), this);
    return m_lazy_constraint_callback.get();
}

idol::Optimizers::impl::CplexBranchCallbackI *idol::CplexCallbackI::create_branch_callback() {
    assert(!m_branch_callback);
    m_branch_callback = std::make_unique<Optimizers::impl::CplexBranchCallbackI>(m_parent.env(), this);
    return m_branch_callback.get();
}

idol::Optimizers::impl::CplexHeuristicCallbackI *idol::CplexCallbackI::create_heuristic_callback() {
    assert(false);
    /*
    assert(!m_heuristic_callback);
    m_heuristic_callback = std::make_unique<Optimizers::impl::CplexHeuristicCallbackI>(m_parent.env(), this);
    return m_heuristic_callback.get();
     */
}

IloRange idol::CplexCallbackI::cplex_temp_constr(const idol::TempCtr &t_temp_ctr) {

    IloExpr expr(m_parent.env());
    for (const auto& [var, constant] : t_temp_ctr.lhs()) {
        expr += constant * m_parent[var];
    }

    switch (t_temp_ctr.type()) {
        case LessOrEqual: return expr <= t_temp_ctr.rhs();
        case GreaterOrEqual: return expr >= t_temp_ctr.rhs();
        case Equal: return expr == t_temp_ctr.rhs();
        default:
            throw Exception("Unknown constraint type");
    }

}

void idol::CplexCallbackI::add_user_cut(const idol::TempCtr &t_user_cut) {

    if (auto* cb = dynamic_cast<Optimizers::impl::CplexUserCutCallbackI*>(m_caller) ; cb) {
        auto cut = cplex_temp_constr(t_user_cut);
        cb->add(cut);
        return;
    }

    throw Exception("Cannot add a user cut at fractional points.");

}

double idol::CplexCallbackI::get_var_local_lb(const idol::Var &t_var) const {
    return m_caller->getLB(m_parent[t_var]);
}

double idol::CplexCallbackI::get_var_local_ub(const idol::Var &t_var) const {
    return m_caller->getUB(m_parent[t_var]);
}

double idol::CplexCallbackI::get_var_primal(const idol::Var &t_var) const {
    return m_caller->getValue(m_parent[t_var]);
}

void idol::CplexCallbackI::set_var_local_lb(const idol::Var &t_var, double t_bound) {
    m_local_bounds_node_id = m_caller->getNodeId();
    m_local_lbs.emplace_back(t_var, t_bound);
}

void idol::CplexCallbackI::set_var_local_ub(const idol::Var &t_var, double t_bound) {
    m_local_bounds_node_id = m_caller->getNodeId();
    m_local_ubs.emplace_back(t_var, t_bound);
}

void idol::Optimizers::impl::CplexLazyConstraintCallbackI::main() {

    try {
        m_callback->call(this, IncumbentSolution);
    } catch (const IloException& t_err) {
        std::cerr << "There was an error during callback, CPLEX reported " << t_err.getMessage() << std::endl;
        throw;
    }

}

void idol::Optimizers::impl::CplexUserCutCallbackI::main() {

    try {
        m_callback->call(this, InvalidSolution);
    } catch (const IloException& t_err) {
        std::cerr << "There was an error during callback, CPLEX reported " << t_err.getMessage() << std::endl;
        throw;
    }

}

void idol::Optimizers::impl::CplexBranchCallbackI::main() {

    const unsigned int n_local_lbs = m_callback->m_local_lbs.size();
    const unsigned int n_local_ubs = m_callback->m_local_ubs.size();
    const unsigned int n_local_bounds = m_callback->m_local_lbs.size() + m_callback->m_local_ubs.size();

    if (n_local_bounds == 0) {
        return;
    }

    assert(*m_callback->m_local_bounds_node_id == getNodeId());

    auto& env = m_callback->m_parent.env();
    IloNumVarArray var_array(env);
    IloNumArray bound_array(env);
    IloCplex::BranchDirectionArray direction_array(env);

    var_array.setSize(n_local_bounds);
    bound_array.setSize(n_local_bounds);
    direction_array.setSize(n_local_bounds);

    for (unsigned int i = 0 ; i < n_local_lbs ; ++i) {
        const auto& [var, lb] = m_callback->m_local_lbs[i];
        var_array[i] = m_callback->m_parent[var];
        bound_array[i] = lb;
        direction_array[i] = IloCplex::BranchUp;
    }

    for (unsigned int i = 0 ; i < n_local_ubs ; ++i) {
        const auto& [var, ub] = m_callback->m_local_ubs[i];
        const unsigned int k = i + n_local_lbs;
        var_array[k] = m_callback->m_parent[var];
        bound_array[k] = ub;
        direction_array[k] = IloCplex::BranchDown;
    }

    makeBranch(var_array, bound_array, direction_array, getObjValue());

    m_callback->m_local_lbs.clear();
    m_callback->m_local_ubs.clear();
    m_callback->m_local_bounds_node_id.reset();
}

#endif
