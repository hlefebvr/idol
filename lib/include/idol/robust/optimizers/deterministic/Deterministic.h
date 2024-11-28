//
// Created by henri on 28.11.24.
//

#ifndef IDOL_DETERMINISTIC_H
#define IDOL_DETERMINISTIC_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/robust/modeling/Description.h"
#include "idol/mixed-integer/modeling/models/Model.h"

namespace idol::Robust {
    class Deterministic;
}

class idol::Robust::Deterministic : public OptimizerFactoryWithDefaultParameters<Deterministic> {
    const Robust::Description& m_description;
    std::unique_ptr<OptimizerFactory> m_deterministic_optimizer;
public:
    explicit Deterministic(const Robust::Description& t_description);

    Deterministic(const Deterministic& t_src);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    Deterministic& with_deterministic_optimizer(const OptimizerFactory& t_deterministic_optimizer);

    static Model make_model(const Model& t_model, const Robust::Description& t_description);
};

#endif //IDOL_DETERMINISTIC_H
