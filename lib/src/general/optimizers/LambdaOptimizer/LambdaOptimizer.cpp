//
// Created by charlotte on 29.11.24.
//
#include "idol/general/optimizers/LambdaOptimizer/LambdaOptimizer.h"
#include "idol/general/optimizers/LambdaOptimizer/Optimizers_LambdaOptimizer.h"

idol::LambdaOptimizer::LambdaOptimizer(const idol::LambdaOptimizer &t_src)
        : OptimizerFactoryWithDefaultParameters<idol::LambdaOptimizer>(t_src), m_lambda(t_src.m_lambda){

}

idol::LambdaOptimizer::LambdaOptimizer(const std::function<void(LambdaContext&)>& t_lambda)
                        : m_lambda(t_lambda){
    if (!m_lambda) {
        throw Exception("No lambda function has been set.");
    }
}

idol::Optimizer *idol::LambdaOptimizer::create(const idol::Model &t_model) const {
    if (!m_lambda) {
        throw Exception("No lambda function has been set.");
    }

    auto* result = new idol::Optimizers::LambdaOptimizer(t_model, m_lambda);

    return result;
}

idol::OptimizerFactory *idol::LambdaOptimizer::clone() const {
    return new LambdaOptimizer(*this);
}

