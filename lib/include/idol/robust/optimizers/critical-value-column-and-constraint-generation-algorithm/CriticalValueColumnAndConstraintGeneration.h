//
// Created by henri on 09.01.26.
//

#ifndef IDOL_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H
#define IDOL_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/robust/modeling/Description.h"

namespace idol::Robust {
    class CriticalValueColumnAndConstraintGeneration;
}

class idol::Robust::CriticalValueColumnAndConstraintGeneration : public OptimizerFactoryWithDefaultParameters<CriticalValueColumnAndConstraintGeneration> {
    const Robust::Description& m_robust_description;
    std::unique_ptr<OptimizerFactory> m_master_optimizer;
    std::unique_ptr<OptimizerFactory> m_separation_optimizer;

    CriticalValueColumnAndConstraintGeneration(const CriticalValueColumnAndConstraintGeneration& t_src);
public:
    CriticalValueColumnAndConstraintGeneration(const Robust::Description& t_description);

    Optimizer* operator()(const Model& t_model) const override;

    [[nodiscard]] OptimizerFactory* clone() const override;

    CriticalValueColumnAndConstraintGeneration& with_master_optimizer(const OptimizerFactory& t_factory);

    CriticalValueColumnAndConstraintGeneration& with_adversarial_optimizer(const OptimizerFactory& t_factory);
};

#endif //IDOL_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H