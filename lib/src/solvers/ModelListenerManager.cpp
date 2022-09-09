//
// Created by henri on 09/09/22.
//
#include "solvers/ModelListenerManager.h"
#include <iostream>
void ModelListenerManager::free(const ModelListener::Id &t_id) {
    m_listeners.erase(t_id.m_id);
}

void ModelListenerManager::add(ModelListener &t_listener) {
    m_listeners.emplace_front(&t_listener);
    t_listener.set_id(ModelListener::Id(*this, m_listeners.begin()));
    t_listener.on_start();
}

#define BROADCAST(x) \
for (auto& ptr_to_listener : m_listeners) { \
    ptr_to_listener->on_##x; \
}

void ModelListenerManager::broadcast_add(const Var &t_var) {
    BROADCAST(add(t_var));
}

void ModelListenerManager::broadcast_remove(const Var &t_var) {
    BROADCAST(remove(t_var));
}

void ModelListenerManager::broadcast_add(const Ctr &t_ctr) {
    BROADCAST(add(t_ctr));
}

void ModelListenerManager::broadcast_remove(const Ctr &t_ctr) {
    BROADCAST(remove(t_ctr));
}

void ModelListenerManager::broadcast_update_objective(const Var &t_var, const Coefficient &t_coeff) {
    BROADCAST(update_objective(t_var, t_coeff));
}

void ModelListenerManager::broadcast_update_rhs(const Ctr &t_ctr, const Coefficient &t_coeff) {
    BROADCAST(update_rhs(t_ctr, t_coeff));
}

void ModelListenerManager::broadcast_update_coefficient(const Ctr &t_ctr, const Var &t_var,
                                                        const Coefficient &t_coefficient) {
    BROADCAST(update_coefficient(t_ctr, t_var, t_coefficient));
}

void ModelListenerManager::broadcast_update_lb(const Var &t_var, double t_lb) {
    BROADCAST(update_lb(t_var, t_lb));
}

void ModelListenerManager::broadcast_update_ub(const Var &t_var, double t_ub) {
    BROADCAST(update_ub(t_var, t_ub));
}

void ModelListenerManager::broadcast_update_type(const Var &t_var, VarType t_type) {
    BROADCAST(update_type(t_var, t_type));
}

void ModelListenerManager::broadcast_update_type(const Ctr &t_ctr, CtrType t_type) {
    BROADCAST(update_type(t_ctr, t_type));
}
