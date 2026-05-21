//
// Created by Henri on 13/05/2026.
//

#ifndef IDOL_ROCPP_WRAPPER_H
#define IDOL_ROCPP_WRAPPER_H

#include "idol/bilevel/modeling/Description.h"
#include "idol/robust/modeling/Description.h"
#include "idol/general/optimizers/OptimizerFactory.h"

namespace idol::Robust {
    class ROCPP;
}

class idol::Robust::ROCPP : public OptimizerFactoryWithDefaultParameters<ROCPP>  {
public:
    enum Approximation { KAdaptability, PiecewiseLinearDR, LinearDR };

    ROCPP(const Robust::Description& t_robust_description, const Bilevel::Description& t_bilevel_description, Approximation t_approximation);

    ROCPP(const ROCPP& t_src);

    ROCPP& with_deterministic_optimizer(const OptimizerFactory& t_optimizer_factory);

    [[nodiscard]] OptimizerFactory* clone() const override;

    static Model make_model(const Model& t_model,
                            const Robust::Description& t_robust_description,
                            const Bilevel::Description& t_bilevel_description,
                            Approximation t_approximation
    );
protected:
    [[nodiscard]] Optimizer* create(const Model& t_model) const override;
private:
    const Bilevel::Description& m_bilevel_description;
    const Robust::Description& m_robust_description;
    std::optional<Approximation> m_approximation;
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;
};

#endif //IDOL_ROCPP_WRAPPER_H