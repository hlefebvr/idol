//
// Created by henri on 13/10/22.
//
#include "../../../include/algorithms/callbacks/Callbacks_RowGeneration.h"
#include "../../../include/algorithms/callbacks/AlgorithmInCallback.h"

Callbacks::RowGeneration::RowGeneration(Algorithm &t_algorithm)
        : ::RowGeneration(*new AlgorithmInCallback(t_algorithm)),
          m_proxy(dynamic_cast<AlgorithmInCallback&>(rmp_solution_strategy())) {

    RowGeneration::set<Attr::MaxIterations>(1);

}

Callbacks::RowGeneration::~RowGeneration() {
    delete &rmp_solution_strategy();
}

void Callbacks::RowGeneration::execute(Context &t_ctx) {
    if (is_in(t_ctx.event(), { NewIncumbentFound, RelaxationSolved })) {

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

void Callbacks::RowGeneration::update_lb(const Var &t_var, double t_lb) {
    ::RowGeneration::update_lb(t_var, t_lb);
}

void Callbacks::RowGeneration::update_ub(const Var &t_var, double t_ub) {
    ::RowGeneration::update_ub(t_var, t_ub);
}
