//
// Created by henri on 23/03/23.
//

#ifndef IDOL_MOSEKOPTIMIZER_H
#define IDOL_MOSEKOPTIMIZER_H

#include "../OptimizerFactory.h"

class MosekOptimizer : public OptimizerFactoryWithDefaultParameters<MosekOptimizer> {
    bool m_continuous_relaxation = false;

    explicit MosekOptimizer(bool t_continuous_relaxation) : m_continuous_relaxation(t_continuous_relaxation) {}
    MosekOptimizer(const MosekOptimizer&) = default;
public:
    MosekOptimizer() = default;
    MosekOptimizer(MosekOptimizer&&) noexcept = default;

    MosekOptimizer& operator=(const MosekOptimizer&) = delete;
    MosekOptimizer& operator=(MosekOptimizer&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    static MosekOptimizer ContinuousRelaxation();

    [[nodiscard]] MosekOptimizer *clone() const override;
};

#endif //IDOL_MOSEKOPTIMIZER_H
