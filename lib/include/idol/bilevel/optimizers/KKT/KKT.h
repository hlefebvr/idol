//
// Created by henri on 29.11.24.
//

#ifndef IDOL_BILEVEL_KKT_H
#define IDOL_BILEVEL_KKT_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/bilevel/optimizers/BilevelOptimizerInterface.h"

namespace idol::Bilevel {
    class KKT;
}

class idol::Bilevel::KKT : public OptimizerFactoryWithDefaultParameters<KKT>, public Bilevel::OptimizerInterface {
    const Bilevel::Description* m_description;
    std::unique_ptr<OptimizerFactory> m_single_level_optimizer;
    std::optional<Annotation<double>> m_big_M;
public:
    KKT() = default;

    explicit KKT(const Bilevel::Description& t_description);

    KKT(const KKT& t_src);

    void set_bilevel_description(const Description &t_bilevel_description) override;

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    KKT& with_single_level_optimizer(const OptimizerFactory& t_deterministic_optimizer);

    KKT& with_big_M(const Annotation<double>& t_big_M);

    KKT& operator+=(const OptimizerFactory& t_optimizer) { return with_single_level_optimizer(t_optimizer); }

    static Model make_model(const Model& t_model, const Bilevel::Description& t_description);

    static Model make_model(const Model& t_model, const Bilevel::Description& t_description, const Annotation<double>& t_big_M);
};

namespace idol {
    static Bilevel::KKT operator+(const Bilevel::KKT& t_strong_duality, const OptimizerFactory& t_optimizer) {
        Bilevel::KKT result(t_strong_duality);
        result += t_optimizer;
        return result;
    }
}

#endif //IDOL_BILEVEL_KKT_H
