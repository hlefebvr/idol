//
// Created by henri on 23/03/23.
//

#ifndef IDOL_GLPKOPTIMIZER_H
#define IDOL_GLPKOPTIMIZER_H

#include "../OptimizerFactory.h"

class GLPKOptimizer : public OptimizerFactory {
    bool m_continuous_relaxation = false;

    explicit GLPKOptimizer(bool t_continuous_relaxation) : m_continuous_relaxation(t_continuous_relaxation) {}
    GLPKOptimizer(const GLPKOptimizer&) = default;
public:
    GLPKOptimizer() = default;
    GLPKOptimizer(GLPKOptimizer&&) noexcept = default;

    GLPKOptimizer& operator=(const GLPKOptimizer&) = delete;
    GLPKOptimizer& operator=(GLPKOptimizer&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    static GLPKOptimizer ContinuousRelaxation();

    [[nodiscard]] GLPKOptimizer *clone() const override;
};


#endif //IDOL_GLPKOPTIMIZER_H
