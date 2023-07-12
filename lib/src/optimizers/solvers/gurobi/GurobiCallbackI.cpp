//
// Created by henri on 31/03/23.
//

#ifdef IDOL_USE_GUROBI

#include "optimizers/solvers/gurobi/GurobiCallbackI.h"
#include "optimizers/solvers/gurobi/Optimizers_Gurobi.h"

idol::GurobiCallbackI::GurobiCallbackI(Optimizers::Gurobi &t_parent) : m_parent(t_parent) {

}

void idol::GurobiCallbackI::add_callback(Callback *t_callback) {
    m_callbacks.emplace_back(t_callback);
}

void idol::GurobiCallbackI::callback() {

    CallbackEvent event;

    switch (where) {
        case GRB_CB_MIPSOL: event = IncumbentSolution; break;
        case GRB_CB_MIPNODE: event = InvalidSolution; break;
        default: return;
    }

    call(event);

}

void idol::GurobiCallbackI::call(CallbackEvent t_event) {

    for (auto& cb : m_callbacks) {
        execute(*cb, t_event);
    }

}

void idol::GurobiCallbackI::add_lazy_cut(const TempCtr &t_lazy_cut) {
    addLazy(gurobi_temp_constr(t_lazy_cut));
}

void idol::GurobiCallbackI::add_user_cut(const TempCtr &t_user_cut) {
    addCut(gurobi_temp_constr(t_user_cut));
}

GRBTempConstr idol::GurobiCallbackI::gurobi_temp_constr(const TempCtr &t_temp_ctr) {

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

idol::Solution::Primal idol::GurobiCallbackI::primal_solution() const {

    auto* me = const_cast<GurobiCallbackI*>(this);

    Solution::Primal result;

    if (where == GRB_CB_MIPSOL) {

        for (const auto& var : m_parent.parent().vars()) {
            auto& impl = m_parent.lazy(var).impl();
            result.set(var, me->getSolution(impl));
        }

        result.set_objective_value(me->getDoubleInfo(GRB_CB_MIPSOL_OBJ));

    } else if (where == GRB_CB_MIPNODE) {

        const auto [status, reason] = m_parent.gurobi_status(me->getIntInfo(GRB_CB_MIPNODE_STATUS));

        if (status != Optimal) {
            result.set_status(status);
            result.set_reason(reason);
            return result;
        }

        for (const auto& var : m_parent.parent().vars()) {
            auto& impl = m_parent.lazy(var).impl();
            result.set(var, me->getNodeRel(impl));
        }

        const auto& obj = m_parent.parent().get_obj_expr();
        double obj_val = 0.;
        for (const auto& [var, constant] : obj.linear()) {
            obj_val += constant.numerical() * result.get(var);
        }

        result.set_objective_value(obj_val);

    } else {
        throw Exception("Primal solution not accessible in this context.");
    }

    result.set_status(Optimal);
    result.set_reason(Proved);

    return result;
}

const idol::Timer &idol::GurobiCallbackI::time() const {
    return m_parent.time();
}

#endif