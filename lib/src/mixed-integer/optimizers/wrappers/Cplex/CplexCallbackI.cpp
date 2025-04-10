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

    std::cout << "Should add " << t_lazy_cut << std::endl;

    auto* cb = dynamic_cast<Optimizers::impl::CplexLazyConstraintCallbackI*>(m_caller);
    if (!cb) {
        throw Exception("Cannot add a lazy callback at fractional points.");
    }

    auto cut = cplex_temp_constr(t_lazy_cut);

    std::cout << "Adding " << cut << std::endl;
    cb->add(cut);
    std::cout << "Added cut" << std::endl;
}

idol::PrimalPoint idol::CplexCallbackI::primal_solution() const {

    std::cout << "Called primal_solution" << std::endl;
    PrimalPoint point;
    point.set_status(Optimal);
    point.set_reason(Proved);
    point.set_objective_value(m_caller->getObjValue());
    for (const auto& var : m_parent.parent().vars()) {
        const auto& cplex_var = m_parent[var];
        point.set(var, m_caller->getValue(cplex_var));
    }

    std::cout << point << std::endl;
    std::cout << "End of primal_solution" << std::endl;

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

void idol::Optimizers::impl::CplexLazyConstraintCallbackI::main() {

    try {
        m_callback->call(this, IncumbentSolution);
    } catch (const IloException& t_err) {
        std::cerr << "There was an error during callback, CPLEX reported " << t_err.getMessage() << std::endl;
        __throw_exception_again;
    }

}

#endif