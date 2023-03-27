//
// Created by henri on 23/03/23.
//

#ifndef IDOL_MOSEKOPTIMIZER_H
#define IDOL_MOSEKOPTIMIZER_H

#ifdef IDOL_USE_MOSEK

#include "../OptimizerFactory.h"

class MosekOptimizer : public OptimizerFactory {
    bool m_continuous_relaxation = false;

    explicit MosekOptimizer(bool t_continuous_relaxation) : m_continuous_relaxation(t_continuous_relaxation) {}
    MosekOptimizer(const MosekOptimizer&) = default;
public:
    MosekOptimizer() = default;
    MosekOptimizer(MosekOptimizer&&) noexcept = default;

    MosekOptimizer& operator=(const MosekOptimizer&) = delete;
    MosekOptimizer& operator=(MosekOptimizer&&) noexcept = delete;

    Backend *operator()(const AbstractModel &t_model) const override;

    static MosekOptimizer ContinuousRelaxation();

    [[nodiscard]] MosekOptimizer *clone() const override;
};

#endif

#endif //IDOL_MOSEKOPTIMIZER_H
