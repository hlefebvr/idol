//
// Created by henri on 31/01/23.
//
#include "optimizers/Optimizer.h"
#include "modeling/models/Model.h"

double impl::Optimizer::as_numeric(const Constant &t_constant) {

    if (!t_constant.is_numerical()) {
        throw Exception("Constant is not numeric.");
    }

    return t_constant.numerical();
}

impl::Optimizer::Optimizer(const Model &t_parent) : m_parent(t_parent) {

}

void impl::Optimizer::optimize() {

    idol_Log(Debug, "Optimizer " << name() << " is getting ready.")

    m_is_terminated = false;
    update();

    idol_Log(Debug, "Optimizer " << name() << " starts.")

    m_timer.start();
    hook_before_optimize();
    hook_optimize();
    hook_after_optimize();
    m_timer.stop();

    idol_Log(Debug, "Optimizer " << name() << " has ended.")

    m_is_terminated = true;

}

double impl::Optimizer::remaining_time() const {
    return std::max(0., m_time_limit - m_timer.count());
}

void impl::Optimizer::terminate() {
    m_is_terminated = true;
}

Optimizer::Optimizer(const Model &t_parent) : impl::Optimizer(t_parent) {

}
