//
// Created by Henri on 15/04/2026.
//

#ifndef IDOL_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H
#define IDOL_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H

#include <idol/general/optimizers/OptimizerFactory.h>

namespace idol::Robust {
    class Description;
    class CriticalValueColumnAndConstraintGeneration;
}

class idol::Robust::CriticalValueColumnAndConstraintGeneration : public OptimizerFactoryWithDefaultParameters<CriticalValueColumnAndConstraintGeneration> {
    const Description& m_description;
    std::unique_ptr<OptimizerFactory> m_master_optimizer_factory;
    std::unique_ptr<OptimizerFactory> m_deterministic_optimizer_factory;
protected:
    [[nodiscard]] Optimizer* create(const Model& t_model) const override;
    CriticalValueColumnAndConstraintGeneration(const CriticalValueColumnAndConstraintGeneration& t_src);
public:
    CriticalValueColumnAndConstraintGeneration(const Robust::Description& t_description);

    [[nodiscard]] OptimizerFactory* clone() const override;

    CriticalValueColumnAndConstraintGeneration& with_master_optimizer(const OptimizerFactory& t_optimizer);
    CriticalValueColumnAndConstraintGeneration& with_deterministic_optimizer(const OptimizerFactory& t_optimizer);
};

#endif //IDOL_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H