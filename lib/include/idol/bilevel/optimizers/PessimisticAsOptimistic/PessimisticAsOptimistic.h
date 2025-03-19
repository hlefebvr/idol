//
// Created by henri on 22.01.25.
//

#ifndef IDOL_PESSIMISTICASOPTIMISTIC_H
#define IDOL_PESSIMISTICASOPTIMISTIC_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/bilevel/optimizers/BilevelOptimizerInterface.h"

namespace idol::Bilevel {
    class PessimisticAsOptimistic;
}

class idol::Bilevel::PessimisticAsOptimistic : public OptimizerFactoryWithDefaultParameters<PessimisticAsOptimistic>, public Bilevel::OptimizerInterface {
    const Bilevel::Description* m_description = nullptr;
    std::unique_ptr<OptimizerFactory> m_optimistic_bilevel_optimizer;
public:
    PessimisticAsOptimistic() = default;

    explicit PessimisticAsOptimistic(const Bilevel::Description& t_description);

    PessimisticAsOptimistic(const PessimisticAsOptimistic& t_src);

    PessimisticAsOptimistic& with_optimistic_bilevel_optimizer(const OptimizerFactory& t_optimizer);

    PessimisticAsOptimistic& operator+=(const OptimizerFactory& t_optimizer) {
        return with_optimistic_bilevel_optimizer(t_optimizer);
    }

    void set_bilevel_description(const Description &t_bilevel_description) override;

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    static std::pair<Model, Bilevel::Description> make_model(const Model &t_model, const Bilevel::Description &t_description);
};

namespace idol {
    static Bilevel::PessimisticAsOptimistic operator+(const Bilevel::PessimisticAsOptimistic& t_pessimistic_as_optimistic, const OptimizerFactory& t_optimizer) {
        Bilevel::PessimisticAsOptimistic result(t_pessimistic_as_optimistic);
        result += t_optimizer;
        return result;
    }
}

#endif //IDOL_PESSIMISTICASOPTIMISTIC_H
