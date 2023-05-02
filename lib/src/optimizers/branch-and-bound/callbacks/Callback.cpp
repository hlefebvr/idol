//
// Created by henri on 12/04/23.
//
#include "optimizers/callbacks/Callback.h"
#include "optimizers/Timer.h"

void Callback::add_user_cut(const TempCtr &t_cut) {
    throw_if_no_interface();
    m_interface->add_user_cut(t_cut);
}

void Callback::add_lazy_cut(const TempCtr &t_cut) {
    throw_if_no_interface();
    m_interface->add_lazy_cut(t_cut);
}

void Callback::throw_if_no_interface() const {
    if (!m_interface) {
        throw Exception("No interface was found.");
    }
}

Solution::Primal Callback::primal_solution() const {
    throw_if_no_interface();
    return m_interface->primal_solution();
}

const Timer &Callback::time() const {
    throw_if_no_interface();
    return m_interface->time();
}

void CallbackI::execute(Callback &t_cb, CallbackEvent t_event) {
    t_cb.m_interface = this;
    t_cb.operator()(t_event);
    t_cb.m_interface = nullptr;
}
