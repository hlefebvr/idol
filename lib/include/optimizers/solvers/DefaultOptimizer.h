//
// Created by henri on 21/03/23.
//

#ifndef IDOL_DEFAULTOPTIMIZER_H
#define IDOL_DEFAULTOPTIMIZER_H

#include "optimizers/OptimizerFactory.h"

template<class BackendT>
class DefaultOptimizer : public OptimizerFactory {
public:
    DefaultOptimizer() = default;

    DefaultOptimizer(const DefaultOptimizer&) = default;
    DefaultOptimizer(DefaultOptimizer&&) noexcept = delete;

    DefaultOptimizer& operator=(const DefaultOptimizer&) = delete;
    DefaultOptimizer& operator=(DefaultOptimizer&&) noexcept = delete;

    BackendT *operator()(const Model &t_model) const override {
        return new BackendT(t_model);
    }

    DefaultOptimizer *clone() const override {
        return new DefaultOptimizer<BackendT>(*this);
    }
};

#endif //IDOL_DEFAULTOPTIMIZER_H
