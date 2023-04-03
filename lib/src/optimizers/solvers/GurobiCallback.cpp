//
// Created by henri on 31/03/23.
//

#ifdef IDOL_USE_GUROBI

#include "optimizers/solvers/GurobiCallback.h"
#include "optimizers/solvers/Optimizers_Gurobi.h"

GurobiCallbackI::GurobiCallbackI(Optimizers::Gurobi &t_parent) : m_parent(t_parent) {

}

void GurobiCallbackI::add_callback(GurobiCallback *t_callback) {
    m_callbacks.emplace_back(t_callback);
}

void GurobiCallbackI::callback() {

    BranchAndBoundEvent event;

    switch (where) {
        case GRB_CB_MIPSOL: event = IncumbentSolution; break;
        case GRB_CB_MIPNODE: event = InvalidSolution; break;
        default: return;
    }

    for (auto& cb : m_callbacks) {

        cb->m_parent = this;
        cb->operator()(event);
        cb->m_parent = nullptr;

    }

}

void GurobiCallbackI::add_lazy_cut(const TempCtr &t_lazy_cut) {
    addLazy(gurobi_temp_constr(t_lazy_cut));
}

void GurobiCallbackI::add_user_cut(const TempCtr &t_user_cut) {
    addCut(gurobi_temp_constr(t_user_cut));
}

GRBTempConstr GurobiCallbackI::gurobi_temp_constr(const TempCtr &t_temp_ctr) {

    const auto& row = t_temp_ctr.row();
    const auto& rhs = m_parent.gurobi_numeric(m_parent.as_numeric(row.rhs())); // NOLINT(readability-static-accessed-through-instance)

    if (!row.quadratic().empty()) {
        throw Exception("Cannot add quadratic cuts in Gurobi.");
    }

    GRBLinExpr lhs;

    for (const auto& [var, constant] : row.linear()) {
        lhs += m_parent.lazy(var).impl() * m_parent.as_numeric(constant); // NOLINT(readability-static-accessed-through-instance)
    }

    switch (t_temp_ctr.type()) {
        case LessOrEqual: return lhs <= rhs;
        case GreaterOrEqual: return lhs >= rhs;
        case Equal: return lhs == rhs;
        default:;
    }

    throw Exception("Unexpected constraint type.");
}

Solution::Primal GurobiCallbackI::primal_solution() const {

    auto* me = const_cast<GurobiCallbackI*>(this);

    Solution::Primal result;

    if (where == GRB_CB_MIPSOL) {

        for (const auto& var : m_parent.parent().vars()) {
            auto& impl = m_parent.lazy(var).impl();
            result.set(var, me->getSolution(impl));
        }

        result.set_objective_value(me->getDoubleInfo(GRB_CB_MIPSOL_OBJ));

    } else if (where == GRB_CB_MIPNODE) {

        for (const auto& var : m_parent.parent().vars()) {
            auto& impl = m_parent.lazy(var).impl();
            result.set(var, me->getNodeRel(impl));
        }

    } else {
        throw Exception("Primal solution not accessible in this context.");
    }

    result.set_status(Optimal);
    result.set_reason(Proved);

    return result;
}

void GurobiCallback::add_lazy_cut(const TempCtr &t_cut) {
    m_parent->add_lazy_cut(t_cut);
}

void GurobiCallback::add_user_cut(const TempCtr &t_user_cut) {
    m_parent->add_user_cut(t_user_cut);
}

Solution::Primal GurobiCallback::primal_solution() const {
    return m_parent->primal_solution();
}

#endif