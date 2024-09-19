//
// Created by henri on 19.09.24.
//

#ifndef IDOL_PENALTYMETHOD_H
#define IDOL_PENALTYMETHOD_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/annotations/Annotation.h"
#include "idol/containers/Map.h"
#include "SubProblem.h"
#include "Formulation.h"
#include "PenaltyUpdates.h"
#include "Optimizers_PADM.h"
#include <optional>

namespace idol {
    class PenaltyMethod;
}

class idol::PenaltyMethod : public OptimizerFactoryWithDefaultParameters<PenaltyMethod> {
public:
    explicit PenaltyMethod(Annotation<Ctr, bool> t_penalized_constraints);

    PenaltyMethod(const PenaltyMethod& t_src);
    PenaltyMethod(PenaltyMethod&&) = default;

    PenaltyMethod& operator=(const PenaltyMethod&) = default;
    PenaltyMethod& operator=(PenaltyMethod&&) = default;

    PenaltyMethod& with_optimizer(const OptimizerFactory& t_optimizer_factory);

    PenaltyMethod& with_rescaling(bool t_rescaling, double t_threshold);

    PenaltyMethod& with_penalty_update(const PenaltyUpdate& t_penalty_update);

    Optimizers::PADM *operator()(const Model &t_model) const override;

    PenaltyMethod *clone() const override;
private:
    Annotation<Var, unsigned int> m_decomposition;
    Annotation<Ctr, bool> m_penalized_constraints;
    std::unique_ptr<OptimizerFactory> m_optimizer;
    std::optional<std::pair<bool, double>> m_rescaling;
    std::unique_ptr<PenaltyUpdate> m_penalty_update;
};

#endif //IDOL_PENALTYMETHOD_H
