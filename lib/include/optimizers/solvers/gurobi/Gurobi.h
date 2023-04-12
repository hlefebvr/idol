//
// Created by henri on 23/03/23.
//

#ifndef IDOL_GUROBI_H
#define IDOL_GUROBI_H

#include "../../OptimizerFactory.h"
#include "../../../../../../../../../usr/include/c++/10/list"
#include "optimizers/callbacks/CallbackFactory.h"

class Callback;

class Gurobi : public OptimizerFactoryWithDefaultParameters<Gurobi> {
    bool m_continuous_relaxation = false;
    bool m_lazy_cuts = false;
    std::list<std::unique_ptr<CallbackFactory>> m_callbacks;

    explicit Gurobi(bool t_continuous_relaxation) : m_continuous_relaxation(t_continuous_relaxation) {}
    Gurobi(const Gurobi& t_src);
public:
    Gurobi() = default;
    Gurobi(Gurobi&&) noexcept = default;

    Gurobi& operator=(const Gurobi&) = delete;
    Gurobi& operator=(Gurobi&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    static Gurobi ContinuousRelaxation();

    Gurobi& with_callback(const CallbackFactory& t_cb);

    Gurobi& with_lazy_cut(bool t_value);

    [[nodiscard]] Gurobi *clone() const override;
};

#endif //IDOL_GUROBI_H
