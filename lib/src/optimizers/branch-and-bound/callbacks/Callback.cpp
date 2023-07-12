//
// Created by henri on 12/04/23.
//
#include "optimizers/callbacks/Callback.h"
#include "optimizers/Timer.h"

void idol::Callback::add_user_cut(const TempCtr &t_cut) {
    throw_if_no_interface();
    m_interface->add_user_cut(t_cut);
}

void idol::Callback::add_lazy_cut(const TempCtr &t_cut) {
    throw_if_no_interface();
    m_interface->add_lazy_cut(t_cut);
}

void idol::Callback::throw_if_no_interface() const {
    if (!m_interface) {
        throw Exception("No interface was found.");
    }
}

idol::Solution::Primal idol::Callback::primal_solution() const {
    throw_if_no_interface();
    return m_interface->primal_solution();
}

const idol::Timer &idol::Callback::time() const {
    throw_if_no_interface();
    return m_interface->time();
}

void idol::CallbackI::execute(Callback &t_cb, CallbackEvent t_event) {
    t_cb.m_interface = this;
    t_cb.operator()(t_event);
    t_cb.m_interface = nullptr;
}
