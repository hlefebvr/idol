//
// Created by henri on 29.11.24.
//

#ifndef IDOL_BILEVEL_KKT_H
#define IDOL_BILEVEL_KKT_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"

namespace idol::Bilevel {
    class KKT;
}

class idol::Bilevel::KKT : public OptimizerFactoryWithDefaultParameters<KKT> {
    const Bilevel::Description& m_description;
    std::unique_ptr<OptimizerFactory> m_single_level_optimizer;
    std::optional<Annotation<double>> m_big_M;
public:
    explicit KKT(const Bilevel::Description& t_description);

    KKT(const KKT& t_src);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    KKT& with_single_level_optimizer(const OptimizerFactory& t_deterministic_optimizer);

    KKT& with_big_M(const Annotation<double>& t_big_M);

    static Model make_model(const Model& t_model, const Bilevel::Description& t_description);

    static Model make_model(const Model& t_model, const Bilevel::Description& t_description, const Annotation<double>& t_big_M);
};

#endif //IDOL_BILEVEL_KKT_H
