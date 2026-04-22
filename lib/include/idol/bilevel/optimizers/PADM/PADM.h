//
// Created by Henri on 22/04/2026.
//

#ifndef IDOL_PADM_H
#define IDOL_PADM_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/bilevel/optimizers/BilevelOptimizerInterface.h"

namespace idol::Bilevel {
    class PADM;
}

class idol::Bilevel::PADM : public OptimizerFactoryWithDefaultParameters<PADM>, public Bilevel::OptimizerInterface {
    const Description* m_description = nullptr;
    std::unique_ptr<OptimizerFactory> m_single_level_optimizer;
protected:
    Optimizer *create(const Model &t_model) const override;
public:
    PADM() = default;

    explicit PADM(const Bilevel::Description& t_description);

    PADM(const PADM& t_src);

    void set_bilevel_description(const Description &t_bilevel_description) override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    PADM& with_single_level_optimizer(const OptimizerFactory& t_deterministic_optimizer);

    PADM& operator+=(const OptimizerFactory& t_optimizer) { return with_single_level_optimizer(t_optimizer); }
};

#endif //IDOL_PADM_H