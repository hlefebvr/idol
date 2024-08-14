//
// Created by henri on 28.06.24.
//

#ifndef IDOL_CONVEXIFICATION_H
#define IDOL_CONVEXIFICATION_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/models/Model.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/modeling/robust-optimization/StageDescription.h"

namespace idol::Robust {
    class Convexification;
}

class idol::Robust::Convexification : public OptimizerFactoryWithDefaultParameters<Convexification> {
    Robust::StageDescription m_stage_description;
    const Model& m_uncertainty_set;

    std::unique_ptr<DantzigWolfeDecomposition> m_dantzig_wolfe_optimizer;
public:
    Convexification(Robust::StageDescription t_stage_description, const Model& t_uncertainty_set);

    Convexification(const Convexification& t_src);

    Optimizer *operator()(const Model &t_model) const override;

    Convexification *clone() const override;

    Convexification& with_optimizer(const DantzigWolfeDecomposition& t_optimizer);
};

#endif //IDOL_CONVEXIFICATION_H
