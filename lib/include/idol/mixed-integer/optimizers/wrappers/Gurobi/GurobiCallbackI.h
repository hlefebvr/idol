//
// Created by henri on 31/03/23.
//

#ifndef CCG_WITH_NESTED_CG_GUROBICALLBACK_H
#define CCG_WITH_NESTED_CG_GUROBICALLBACK_H


#include <memory>
#include <list>
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/mixed-integer/optimizers/callbacks/Callback.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/headers/header_gurobi.h"

namespace idol {
    namespace Optimizers {
        class Gurobi;
    }
    namespace impl {
        static int grb_callback(GRBmodel *t_model, void *t_cbdata, int t_where, void *t_usrdata);
    }
    class GurobiCallbackI;
}

class idol::GurobiCallbackI : public CallbackI {

    Optimizers::Gurobi& m_parent;
    std::list<std::unique_ptr<Callback>> m_callbacks;

    int m_where = 0;
    void* m_cbdata = nullptr;
    double m_best_obj = Inf;
    double m_best_bound = -Inf;

    struct GurobiTempConstr {
        std::vector<int> indices;
        std::vector<double> coefficients;
        char sense = 0;
        double rhs = 0;
    };

    GurobiTempConstr gurobi_temp_constr(const TempCtr& t_temp_ctr) const;
public:
    explicit GurobiCallbackI(Optimizers::Gurobi& t_parent);

    void add_callback(Callback* t_callback);
    void call();

    friend int idol::impl::grb_callback(GRBmodel *t_model, void *t_cbdata, int t_where, void *t_usrdata);
protected:
    [[nodiscard]] const Model& original_model() const override;
    void add_lazy_cut(const TempCtr& t_lazy_cut) override;
    void add_user_cut(const TempCtr& t_user_cut) override;
    void submit_heuristic_solution(PrimalPoint t_solution) override;
    [[nodiscard]] PrimalPoint primal_solution() const override;
    [[nodiscard]] const Timer &time() const override;
    [[nodiscard]] double best_obj() const override;
    [[nodiscard]] double best_bound() const override;
    void terminate() override;
};

#endif //CCG_WITH_NESTED_CG_GUROBICALLBACK_H
