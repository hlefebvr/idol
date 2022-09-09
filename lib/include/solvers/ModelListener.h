//
// Created by henri on 09/09/22.
//

#ifndef OPTIMIZE_MODELLISTENER_H
#define OPTIMIZE_MODELLISTENER_H

#include <list>
#include "../modeling/Types.h"

class Var;
class Ctr;
class Coefficient;
class ModelListenerManager;

class ModelListener {
    friend class ModelListenerManager;
protected:
    ModelListener() = default;

    virtual ~ModelListener();

    virtual void on_start() {}
    virtual void on_add(const Var& t_var) {}
    virtual void on_remove(const Var& t_var) {}
    virtual void on_add(const Ctr& t_ctr) {}
    virtual void on_remove(const Ctr& t_ctr) {}
    virtual void on_update_objective(const Var& t_var, const Coefficient& t_coeff) {}
    virtual void on_update_rhs(const Ctr& t_ctr, const Coefficient& t_coeff) {}
    virtual void on_update_coefficient(const Ctr& t_ctr, const Var& t_var, const Coefficient& t_coefficient) {}
    virtual void on_update_lb(const Var& t_var, double t_lb) {}
    virtual void on_update_ub(const Var& t_var, double t_ub) {}
    virtual void on_update_type(const Var& t_var, VarType t_type) {}
    virtual void on_update_type(const Ctr& t_ctr, CtrType t_type) {}

    class Id {
        friend class ::ModelListenerManager;
        ModelListenerManager* m_manager;
        typename std::list<ModelListener*>::iterator m_id;
        explicit Id(ModelListenerManager& t_manager, std::list<ModelListener*>::iterator&& t_it);
    public:
        Id(const Id&) = default;
        Id(Id&&) noexcept = default;

        Id& operator=(const Id&) = default;
        Id& operator=(Id&&) noexcept = default;

        void free();
    };

private:
    std::list<Id> m_ids;
    void set_id(const Id& t_id) { m_ids.emplace_back(t_id); }
};


#endif //OPTIMIZE_MODELLISTENER_H
