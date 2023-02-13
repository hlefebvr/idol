//
// Created by henri on 13/10/22.
//
#include "../../../include/algorithms/callbacks/Callbacks_RowGeneration.h"
#include "modeling/variables/Attributes_Var.h"

Callbacks::RowGeneration::~RowGeneration() {
    delete &rmp_solution_strategy();
}

void Callbacks::RowGeneration::execute(Context &t_ctx) {
    if (is_in(t_ctx.event(), m_events)) {

        m_proxy.set_in_callback(true);
        m_proxy.set_context(&t_ctx);

        RowGeneration::solve();

        m_proxy.set_in_callback(false);
        m_proxy.set_context(nullptr);
    }
}

RowGenerationSP &Callbacks::RowGeneration::add_subproblem(const Ctr &t_ctr) {
    return ::RowGeneration::add_subproblem(t_ctr);
}

Solution::Primal Callbacks::RowGeneration::help() const {
    Solution::Primal result;
    for (auto& subproblem : m_subproblems) {
        result.merge_without_conflict(subproblem.primal_solution());
    }
    return result;

}

void Callbacks::RowGeneration::update_var_lb(const Var &t_var, double t_lb) {
    ::RowGeneration::set(Attr::Var::Lb, t_var, t_lb);
}

void Callbacks::RowGeneration::update_var_ub(const Var &t_var, double t_ub) {
    ::RowGeneration::set(Attr::Var::Ub, t_var, t_ub);
}
