//
// Created by henri on 23/03/23.
//

#ifndef IDOL_GUROBI_H
#define IDOL_GUROBI_H

#include "../../OptimizerFactory.h"
#include <list>
#include <memory>
#include "optimizers/callbacks/CallbackFactory.h"

class Callback;

class Gurobi : public OptimizerFactoryWithDefaultParameters<Gurobi> {
    std::optional<bool> m_continuous_relaxation;
    std::optional<bool> m_lazy_cuts;
    std::optional<bool> m_use_cuts;
    std::optional<bool> m_use_heuristics;
    std::optional<bool> m_nonconvexities;
    std::optional<bool> m_use_dual_reduction;
    std::optional<unsigned int> m_max_n_solution_in_pool;
    std::list<std::unique_ptr<CallbackFactory>> m_callbacks;

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

    Gurobi& with_cutting_planes(bool t_value);

    Gurobi& with_heuristics(bool t_value);

    Gurobi& with_max_n_solution_in_pool(unsigned int t_value);

    Gurobi& with_continuous_relaxation_only(bool t_value);

    Gurobi& with_nonconvexities(bool t_value);

    Gurobi& with_dual_reductions(bool t_value);

    [[nodiscard]] Gurobi *clone() const override;
};

#endif //IDOL_GUROBI_H
