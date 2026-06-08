//
// Created by charlotte on 05.06.2026.
//

#ifndef IDOL_LAMBDA_CONTEXT_H
#define IDOL_LAMBDA_CONTEXT_H

#include "idol/general/utils/types.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/general/optimizers/LambdaOptimizer/Optimizers_LambdaOptimizer.h"

namespace idol {
    class LambdaContext {
        idol::Optimizers::LambdaOptimizer& m_optimizer;
    public:
        explicit LambdaContext(idol::Optimizers::LambdaOptimizer& t_optimizer)
        : m_optimizer(t_optimizer){
            
        }

        void set_status(idol::SolutionStatus t_status){
            std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
            m_optimizer.set_status(t_status);
        }

        idol::SolutionStatus get_status() const{
            std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
            return m_optimizer.get_status();
        }

        idol::Model& get_model(){
            return m_optimizer.get_model();
        }
    };
}

#endif // IDOL_LAMBDA_CONTEXT_H