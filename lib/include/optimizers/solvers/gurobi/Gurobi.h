//
// Created by henri on 23/03/23.
//

#ifndef IDOL_GUROBI_H
#define IDOL_GUROBI_H

#include "../../OptimizerFactory.h"
#include <list>
#include <memory>
#include "optimizers/callbacks/CallbackFactory.h"
#include "../../../containers/Map.h"

#ifdef IDOL_USE_GUROBI
#include <gurobi_c++.h>
#else
enum GRB_IntParam {};
enum GRB_DoubleParam {};
#endif

namespace idol {
    class Callback;
    class Gurobi;
}

class idol::Gurobi : public OptimizerFactoryWithDefaultParameters<Gurobi> {
    std::optional<bool> m_continuous_relaxation;
    std::optional<bool> m_lazy_cuts;
    std::optional<unsigned int> m_max_n_solution_in_pool;
    std::list<std::unique_ptr<CallbackFactory>> m_callbacks;

    Map<GRB_IntParam, int> m_int_params;
    Map<GRB_DoubleParam, double> m_double_params;

    Gurobi(const Gurobi& t_src);
public:
    Gurobi() = default;
    Gurobi(Gurobi&&) noexcept = default;

    Gurobi& operator=(const Gurobi&) = delete;
    Gurobi& operator=(Gurobi&&) noexcept = default;

    Optimizer *operator()(const Model &t_model) const override;

    static Gurobi ContinuousRelaxation();

    Gurobi& with_callback(const CallbackFactory& t_cb);

    Gurobi& with_lazy_cut(bool t_value);

    Gurobi& with_max_n_solution_in_pool(unsigned int t_value);

    Gurobi& with_continuous_relaxation_only(bool t_value);

    Gurobi& with_external_param(GRB_IntParam t_param, int t_value);

    Gurobi& with_external_param(GRB_DoubleParam t_param, double t_value);

    [[nodiscard]] Gurobi *clone() const override;
};

#endif //IDOL_GUROBI_H
