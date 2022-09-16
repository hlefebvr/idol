//
// Created by henri on 09/09/22.
//
#include "modeling/models/ListenerManager.h"

void ListenerManager::free(const Listener::Id &t_id) {
    m_listeners.erase(t_id.m_id);
}

void ListenerManager::add(Listener &t_listener) {
    m_listeners.emplace_front(&t_listener);
    t_listener.set_id(Listener::Id(*this, m_listeners.begin()));
    t_listener.on_start();
}

#define BROADCAST(x) \
for (auto& ptr_to_listener : m_listeners) { \
    ptr_to_listener->on_##x; \
}

void ListenerManager::broadcast_add(const Var &t_var) {
    BROADCAST(add(t_var));
}

void ListenerManager::broadcast_remove(const Var &t_var) {
    BROADCAST(remove(t_var));
}

void ListenerManager::broadcast_add(const Ctr &t_ctr) {
    BROADCAST(add(t_ctr));
}

void ListenerManager::broadcast_remove(const Ctr &t_ctr) {
    BROADCAST(remove(t_ctr));
}

void ListenerManager::broadcast_update_objective(const Var &t_var, const Coefficient &t_coeff) {
    BROADCAST(update_objective(t_var, t_coeff));
}

void ListenerManager::broadcast_update_rhs(const Ctr &t_ctr, const Coefficient &t_coeff) {
    BROADCAST(update_rhs(t_ctr, t_coeff));
}

void ListenerManager::broadcast_update_coefficient(const Ctr &t_ctr, const Var &t_var,
                                                   const Coefficient &t_coefficient) {
    BROADCAST(update_coefficient(t_ctr, t_var, t_coefficient));
}

void ListenerManager::broadcast_update_lb(const Var &t_var, double t_lb) {
    BROADCAST(update_lb(t_var, t_lb));
}

void ListenerManager::broadcast_update_ub(const Var &t_var, double t_ub) {
    BROADCAST(update_ub(t_var, t_ub));
}

void ListenerManager::broadcast_update_type(const Var &t_var, VarType t_type) {
    BROADCAST(update_type(t_var, t_type));
}

void ListenerManager::broadcast_update_type(const Ctr &t_ctr, CtrType t_type) {
    BROADCAST(update_type(t_ctr, t_type));
}

void ListenerManager::broadcast_update_objective_offset(const Coefficient &t_coeff) {
    BROADCAST(update_objective_offset(t_coeff));
}
