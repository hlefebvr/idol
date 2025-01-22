//
// Created by henri on 22.01.25.
//

#ifndef IDOL_PESSIMISTICASOPTIMISTIC_H
#define IDOL_PESSIMISTICASOPTIMISTIC_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"

namespace idol::Bilevel {
    class PessimisticAsOptimistic;
}

class idol::Bilevel::PessimisticAsOptimistic : public OptimizerFactoryWithDefaultParameters<PessimisticAsOptimistic> {
    //const Bilevel::Description &m_description;
    //std::unique_ptr<OptimizerFactory> m_single_level_optimizer;
public:

    static std::pair<Model, Bilevel::Description> make_model(const Model &t_model, const Bilevel::Description &t_description);
};

#endif //IDOL_PESSIMISTICASOPTIMISTIC_H
