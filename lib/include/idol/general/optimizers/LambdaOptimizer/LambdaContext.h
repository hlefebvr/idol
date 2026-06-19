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
        Optimizers::LambdaOptimizer& m_optimizer;
    public:
        explicit LambdaContext(idol::Optimizers::LambdaOptimizer& t_optimizer)
        : m_optimizer(t_optimizer){
            
        }

        Model get_model() const {
            return m_optimizer.parent().copy();
        }

        void set_status(const SolutionStatus& t_status){
            m_optimizer.set_status(t_status);
        }

        void set_status(const SolutionStatus& t_status) const {
            m_optimizer.set_status(t_status);
        }

        void set_best_obj(double t_best_obj) const {
            m_optimizer.set_best_obj(t_best_obj);
        }

        void set_best_bound(double t_best_bound) const {
            m_optimizer.set_best_bound(t_best_bound);
        }

        void set_var_primal(const Var& t_var, double t_value) const {
            m_optimizer.set_var_primal(t_var, t_value);
        }

    };
}

#endif // IDOL_LAMBDA_CONTEXT_H