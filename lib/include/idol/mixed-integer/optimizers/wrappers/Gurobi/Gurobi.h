//
// Created by henri on 23/03/23.
//

#ifndef IDOL_GUROBI_H
#define IDOL_GUROBI_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include <list>
#include <memory>
#include "idol/mixed-integer/optimizers/callbacks/CallbackFactory.h"
#include "idol/general/utils/Map.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

namespace idol {
    class Callback;
    class Gurobi;
}

class idol::Gurobi : public OptimizerFactoryWithDefaultParameters<Gurobi> {
    std::optional<bool> m_continuous_relaxation;
    std::optional<bool> m_lazy_cuts;
    std::optional<unsigned int> m_max_n_solution_in_pool;
    std::list<std::unique_ptr<CallbackFactory>> m_callbacks;

    Map<std::string, int> m_int_params;
    Map<std::string, double> m_double_params;
public:
    Gurobi() = default;

    Gurobi(const Gurobi& t_src);
    Gurobi(Gurobi&&) noexcept = default;

    Gurobi& operator=(const Gurobi&) = delete;
    Gurobi& operator=(Gurobi&&) noexcept = default;

    Optimizer *operator()(const Model &t_model) const override;

    static Gurobi ContinuousRelaxation();

    Gurobi& add_callback(const CallbackFactory& t_cb);
    Gurobi& with_lazy_cut(bool t_value);
    Gurobi& with_max_n_solution_in_pool(unsigned int t_value);
    Gurobi& with_continuous_relaxation_only(bool t_value);
    Gurobi& with_external_param(const std::string& t_param, int t_value);
    Gurobi& with_external_param(const std::string& t_param, double t_value);

    [[nodiscard]] Gurobi *clone() const override;
};

#endif //IDOL_GUROBI_H
