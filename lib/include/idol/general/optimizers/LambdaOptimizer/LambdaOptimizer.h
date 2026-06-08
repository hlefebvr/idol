//
// Created by charlotte on 05.06.2026.
//

#ifndef IDOL_LAMBDA_OPTIMIZER_H
#define IDOL_LAMBDA_OPTIMIZER_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"


namespace idol {
        class LambdaOptimizer;
        class LambdaContext;
}

class idol::LambdaOptimizer : public OptimizerFactoryWithDefaultParameters<LambdaOptimizer> {
    std::function<void(LambdaContext&, const Model&)> m_lambda;
    std::unique_ptr<LambdaContext> m_lambda_context;
protected:
    Optimizer *create(const Model &t_model) const override;
public:
    LambdaOptimizer() = default;

    explicit LambdaOptimizer(const std::function<void(LambdaContext&, const Model&)>& t_lambda);

    explicit LambdaOptimizer(const LambdaOptimizer& t_src);

    [[nodiscard]] OptimizerFactory *clone() const override;
};

#endif //IDOL_LAMBDA_OPTIMIZER_H
