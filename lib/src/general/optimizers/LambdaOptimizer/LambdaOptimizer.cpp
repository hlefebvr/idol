//
// Created by henri on 29.11.24.
//
#include "idol/general/optimizers/LambdaOptimizer/LambdaOptimizer.h"
#include "idol/general/optimizers/LambdaOptimizer/Optimizers_LambdaOptimizer.h"

idol::LambdaOptimizer::LambdaOptimizer(const idol::LambdaOptimizer &t_src)
        : OptimizerFactoryWithDefaultParameters<idol::LambdaOptimizer>(t_src){
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 

}

idol::LambdaOptimizer::LambdaOptimizer(const std::function<void(LambdaContext&)>& t_lambda)
                        : m_lambda(t_lambda){
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    if (!m_lambda) {
        throw Exception("No lambda function has been set.");
    }

    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
}

idol::Optimizer *idol::LambdaOptimizer::create(const idol::Model &t_model) const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    if (!m_lambda) {
        throw Exception("No lambda function has been set.");
    }

    std::cout << t_model.get_obj_expr() << std::endl;
    auto* result = new idol::Optimizers::LambdaOptimizer(t_model, m_lambda);

    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    return result;
}

idol::OptimizerFactory *idol::LambdaOptimizer::clone() const {
    std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    std::cout << __FILE__ << " " << "END " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
    return new LambdaOptimizer(*this);
}

