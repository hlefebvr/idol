//
// Created by henri on 31/03/23.
//

#ifndef CCG_WITH_NESTED_CG_GUROBICALLBACK_H
#define CCG_WITH_NESTED_CG_GUROBICALLBACK_H

#ifdef IDOL_USE_GUROBI

#include <gurobi_c++.h>
#include <memory>
#include <list>
#include "modeling/constraints/TempCtr.h"
#include "optimizers/branch-and-bound/callbacks/CallbackI.h"

namespace Optimizers {
    class Gurobi;
}

class GurobiCallbackI;

class GurobiCallback {
    friend class GurobiCallbackI;
    GurobiCallbackI* m_parent = nullptr;
public:
    virtual ~GurobiCallback() = default;
protected:
    virtual void operator()(BranchAndBoundEvent t_event) = 0;

    void add_lazy_cut(const TempCtr& t_lazy_cut);

    void add_user_cut(const TempCtr& t_user_cut);

    [[nodiscard]] Solution::Primal primal_solution() const;
};

class GurobiCallbackI : public GRBCallback {
    friend class GurobiCallback;

    Optimizers::Gurobi& m_parent;
    std::list<std::unique_ptr<GurobiCallback>> m_callbacks;

    GRBTempConstr gurobi_temp_constr(const TempCtr& t_temp_ctr);
public:
    explicit GurobiCallbackI(Optimizers::Gurobi& t_parent);

    void add_callback(GurobiCallback* t_callback);
protected:
    void callback() override;

    void add_lazy_cut(const TempCtr& t_lazy_cut);

    void add_user_cut(const TempCtr& t_user_cut);

    [[nodiscard]] Solution::Primal primal_solution() const;
};

#endif

#endif //CCG_WITH_NESTED_CG_GUROBICALLBACK_H