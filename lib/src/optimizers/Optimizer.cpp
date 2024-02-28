//
// Created by henri on 31/01/23.
//
#include "idol/optimizers/Optimizer.h"
#include "idol/modeling//models/Model.h"

idol::impl::Optimizer::Optimizer(const Model &t_parent) : m_parent(t_parent) {

}

void idol::impl::Optimizer::optimize() {

    m_is_terminated = false;
    update();

    m_timer.start();
    hook_before_optimize();
    hook_optimize();
    hook_after_optimize();
    m_timer.stop();

    m_is_terminated = true;

}

double idol::impl::Optimizer::get_remaining_time() const {
    return std::max(0., m_param_time_limit - m_timer.count());
}

void idol::impl::Optimizer::terminate() {
    m_is_terminated = true;
}

double idol::impl::Optimizer::get_param_best_obj_stop() const {

    if (m_param_best_obj_stop.has_value()) {
        return m_param_best_obj_stop.value();
    }

    return m_parent.get_obj_sense() == Minimize ? -Inf : Inf;
}

double idol::impl::Optimizer::get_param_best_bound_stop() const {

    if (m_param_best_bound_stop.has_value()) {
        return m_param_best_bound_stop.value();
    }

    return m_parent.get_obj_sense() == Minimize ? Inf : -Inf;

}

idol::Optimizer::Optimizer(const Model &t_parent) : impl::Optimizer(t_parent) {

}
