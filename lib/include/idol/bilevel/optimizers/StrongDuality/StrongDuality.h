//
// Created by henri on 29.11.24.
//

#ifndef IDOL_BILEVEL_STRONG_DUALITY_H
#define IDOL_BILEVEL_STRONG_DUALITY_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"

namespace idol::Bilevel {
    class StrongDuality;
}

class idol::Bilevel::StrongDuality : public OptimizerFactoryWithDefaultParameters<StrongDuality> {
    const Bilevel::Description& m_description;
    std::unique_ptr<OptimizerFactory> m_single_level_optimizer;
public:
    explicit StrongDuality(const Bilevel::Description& t_description);

    StrongDuality(const StrongDuality& t_src);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    StrongDuality& with_single_level_optimizer(const OptimizerFactory& t_deterministic_optimizer);

    StrongDuality& operator+=(const OptimizerFactory& t_optimizer) { return with_single_level_optimizer(t_optimizer); }

    static Model make_model(const Model& t_model, const Bilevel::Description& t_description);
};

namespace idol {
    static Bilevel::StrongDuality operator+(const Bilevel::StrongDuality& t_strong_duality, const OptimizerFactory& t_optimizer) {
        Bilevel::StrongDuality result(t_strong_duality);
        result += t_optimizer;
        return result;
    }
}

#endif //IDOL_BILEVEL_STRONG_DUALITY_H
