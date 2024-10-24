//
// Created by henri on 12/04/23.
//
#include "idol/optimizers/mixed-integer-optimization/callbacks/Callback.h"
#include "idol/optimizers/Timer.h"

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

idol::PrimalPoint idol::Callback::primal_solution() const {
    throw_if_no_interface();
    return m_interface->primal_solution();
}

const idol::Timer &idol::Callback::time() const {
    throw_if_no_interface();
    return m_interface->time();
}

const idol::Model &idol::Callback::original_model() const {
    throw_if_no_interface();
    return m_interface->original_model();
}

void idol::Callback::submit_heuristic_solution(const idol::PrimalPoint &t_solution) {
    throw_if_no_interface();
    m_interface->submit_heuristic_solution(t_solution);
}

double idol::Callback::best_obj() const {
    throw_if_no_interface();
    return m_interface->best_obj();
}

double idol::Callback::best_bound() const {
    throw_if_no_interface();
    return m_interface->best_bound();
}

void idol::Callback::terminate() {
    throw_if_no_interface();
    m_interface->terminate();
}

void idol::CallbackI::execute(Callback &t_cb, CallbackEvent t_event) {
    t_cb.m_interface = this;
    t_cb.operator()(t_event);
    t_cb.m_interface = nullptr;
}
