//
// Created by henri on 23/03/23.
//

#ifndef IDOL_GUROBIOPTIMIZER_H
#define IDOL_GUROBIOPTIMIZER_H

#include "../OptimizerFactory.h"

class GurobiOptimizer : public OptimizerFactory {
    bool m_continuous_relaxation = false;

    explicit GurobiOptimizer(bool t_continuous_relaxation) : m_continuous_relaxation(t_continuous_relaxation) {}
    GurobiOptimizer(const GurobiOptimizer&) = default;
public:
    GurobiOptimizer() = default;
    GurobiOptimizer(GurobiOptimizer&&) noexcept = default;

    GurobiOptimizer& operator=(const GurobiOptimizer&) = delete;
    GurobiOptimizer& operator=(GurobiOptimizer&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    static GurobiOptimizer ContinuousRelaxation();

    [[nodiscard]] GurobiOptimizer *clone() const override;
};

#endif //IDOL_GUROBIOPTIMIZER_H
