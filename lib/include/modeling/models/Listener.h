//
// Created by henri on 09/09/22.
//

#ifndef OPTIMIZE_LISTENER_H
#define OPTIMIZE_LISTENER_H

#include <list>
#include "modeling/Types.h"

class Var;
class Ctr;
class Constant;
class ListenerManager;

class Listener {
protected:
    Listener() = default;

    virtual ~Listener();

    virtual void on_start() {}
    virtual void on_add(const Var& t_var) {}
    virtual void on_remove(const Var& t_var) {}
    virtual void on_add(const Ctr& t_ctr) {}
    virtual void on_remove(const Ctr& t_ctr) {}
    virtual void on_update_objective(const Var& t_var, const Constant& t_coeff) {}
    virtual void on_update_objective_offset(const Constant& t_coeff) {}
    virtual void on_update_rhs(const Ctr& t_ctr, const Constant& t_coeff) {}
    virtual void on_update_coefficient(const Ctr& t_ctr, const Var& t_var, const Constant& t_coefficient) {}
    virtual void on_update_lb(const Var& t_var, double t_lb) {}
    virtual void on_update_ub(const Var& t_var, double t_ub) {}
    virtual void on_update_type(const Var& t_var, VarType t_type) {}
    virtual void on_update_type(const Ctr& t_ctr, CtrType t_type) {}

    class Id {
        friend class ::ListenerManager;
        ListenerManager* m_manager;
        typename std::list<Listener*>::iterator m_id;
        explicit Id(ListenerManager& t_manager, std::list<Listener*>::iterator&& t_it);
    public:
        Id(const Id&) = default;
        Id(Id&&) noexcept = default;

        Id& operator=(const Id&) = default;
        Id& operator=(Id&&) noexcept = default;

        void free();
    };

private:
    friend class ListenerManager;
    std::list<Id> m_ids;
    void set_id(const Id& t_id) { m_ids.emplace_back(t_id); }
};


#endif //OPTIMIZE_LISTENER_H
