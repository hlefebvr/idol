//
// Created by henri on 09/09/22.
//

#ifndef OPTIMIZE_LISTENERMANAGER_H
#define OPTIMIZE_LISTENERMANAGER_H

#include <list>
#include <memory>
#include "Listener.h"
#include "modeling/Types.h"

class Var;
class Ctr;
class Constant;

class ListenerManager;

class ListenerManager {
    std::list<Listener*> m_listeners;
public:
    void broadcast_add(const Var& t_var);
    void broadcast_remove(const Var& t_var);
    void broadcast_add(const Ctr& t_ctr);
    void broadcast_remove(const Ctr& t_ctr);
    void broadcast_update_objective(const Var& t_var, const Constant& t_coeff);
    void broadcast_update_objective_offset(const Constant& t_coeff);
    void broadcast_update_rhs(const Ctr& t_ctr, const Constant& t_coeff);
    void broadcast_update_coefficient(const Ctr& t_ctr, const Var& t_var, const Constant& t_coefficient);
    void broadcast_update_lb(const Var& t_var, double t_lb);
    void broadcast_update_ub(const Var& t_var, double t_ub);
    void broadcast_update_type(const Var& t_var, VarType t_type);
    void broadcast_update_type(const Ctr& t_ctr, CtrType t_type);

    void add(Listener& t_listener);

    void free(const Listener::Id& t_id);
};

#endif //OPTIMIZE_LISTENERMANAGER_H
