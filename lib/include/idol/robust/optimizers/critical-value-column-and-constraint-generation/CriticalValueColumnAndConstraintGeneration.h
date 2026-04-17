//
// Created by Henri on 15/04/2026.
//

#ifndef IDOL_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H
#define IDOL_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H

namespace idol::Robust {
    class CriticalValueColumnAndConstraintGeneration;
}

class idol::Robust::CriticalValueColumnAndConstraintGeneration : public OptimizerFactoryWithDefaultParameters<CriticalValueColumnAndConstraintGeneration> {
    const Robust::Description& m_description;
protected:
    [[nodiscard]] Optimizer* create(const Model& t_model) const override;
public:
    CriticalValueColumnAndConstraintGeneration(const Robust::Description& t_description);

    Optimizer* operator()(const Model& t_model) const override;
    [[nodiscard]] OptimizerFactory* clone() const override;
};


#endif //IDOL_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H