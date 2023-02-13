//
// Created by henri on 13/10/22.
//
#include "../../../include/algorithms/callbacks/Callback.h"

Solution::Primal Callback::Context::node_primal_solution() const {
    throw NotImplemented("Retrieving primal solution from callback", "primal_solution");
}

Solution::Dual Callback::Context::node_dual_solution() const {
    throw NotImplemented("Retrieving dual solution from callback", "dual_solution");
}

Ctr Callback::Context::add_lazy_cut(TempCtr t_ctr) {
    throw NotImplemented("Adding a lazy cut from callback", "add_lazy_cut");
}

bool Callback::Context::submit_solution(Solution::Primal &&t_solution) {
    throw NotImplemented("Submitting a solution from callback", "submit_solution");
}
