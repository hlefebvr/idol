//
// Created by henri on 31/03/23.
//

#ifndef CCG_WITH_NESTED_CG_GUROBICALLBACK_H
#define CCG_WITH_NESTED_CG_GUROBICALLBACK_H

#ifdef IDOL_USE_GUROBI

#include <gurobi_c++.h>
#include <memory>
#include <list>
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/mixed-integer/optimizers/callbacks/Callback.h"

namespace idol {
    namespace Optimizers {
        class Gurobi;
    }
    class GurobiCallbackI;
}

class idol::GurobiCallbackI : public GRBCallback, public CallbackI {

    Optimizers::Gurobi& m_parent;
    std::list<std::unique_ptr<Callback>> m_callbacks;

    GRBTempConstr gurobi_temp_constr(const TempCtr& t_temp_ctr);
public:
    explicit GurobiCallbackI(Optimizers::Gurobi& t_parent);

    void add_callback(Callback* t_callback);

    void call(CallbackEvent t_event);
protected:
    void callback() override;

    [[nodiscard]] const Model& original_model() const override;

    void add_lazy_cut(const TempCtr& t_lazy_cut) override;

    void add_user_cut(const TempCtr& t_user_cut) override;

    void submit_heuristic_solution(PrimalPoint t_solution) override;

    [[nodiscard]] PrimalPoint primal_solution() const override;

    [[nodiscard]] const Timer &time() const override;

    double best_obj() const override;

    double best_bound() const override;

    void terminate() override;
};

#endif

#endif //CCG_WITH_NESTED_CG_GUROBICALLBACK_H
