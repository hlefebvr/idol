//
// Created by henri on 23/03/23.
//

#ifndef IDOL_GUROBI_H
#define IDOL_GUROBI_H

#include "../OptimizerFactory.h"

class Gurobi : public OptimizerFactoryWithDefaultParameters<Gurobi> {
    bool m_continuous_relaxation = false;

    explicit Gurobi(bool t_continuous_relaxation) : m_continuous_relaxation(t_continuous_relaxation) {}
    Gurobi(const Gurobi&) = default;
public:
    Gurobi() = default;
    Gurobi(Gurobi&&) noexcept = default;

    Gurobi& operator=(const Gurobi&) = delete;
    Gurobi& operator=(Gurobi&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    static Gurobi ContinuousRelaxation();

    [[nodiscard]] Gurobi *clone() const override;
};

#endif //IDOL_GUROBI_H
