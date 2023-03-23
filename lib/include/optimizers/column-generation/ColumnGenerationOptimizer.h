//
// Created by henri on 22/03/23.
//

#ifndef IDOL_COLUMNGENERATIONOPTIMIZER_H
#define IDOL_COLUMNGENERATIONOPTIMIZER_H

#include "optimizers/OptimizerFactory.h"
#include "ColumnGeneration.h"

class ColumnGenerationOptimizer : public OptimizerFactory {
    std::unique_ptr<OptimizerFactory> m_master_optimizer;
    std::unique_ptr<OptimizerFactory> m_pricing_optimizer;
protected:
    ColumnGenerationOptimizer(const ColumnGenerationOptimizer& t_src)
        : m_master_optimizer(t_src.m_master_optimizer->clone()),
          m_pricing_optimizer(t_src.m_pricing_optimizer->clone()) {}
public:
    explicit ColumnGenerationOptimizer(const OptimizerFactory& t_master_optimizer,
                                       const OptimizerFactory& t_pricing_optimizer)
        : m_master_optimizer(t_master_optimizer.clone()),
          m_pricing_optimizer(t_pricing_optimizer.clone()) {}

    Backend *operator()(const AbstractModel &t_model) const override {
        return new ColumnGeneration(t_model.as<BlockModel<Ctr>>(),
                                    *m_master_optimizer,
                                    *m_pricing_optimizer);
    }

    [[nodiscard]] ColumnGenerationOptimizer *clone() const override {
        return new ColumnGenerationOptimizer(*this);
    }
};

#endif //IDOL_COLUMNGENERATIONOPTIMIZER_H
