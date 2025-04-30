//
// Created by henri on 29.11.24.
//

#ifndef IDOL_BILEVEL_KKT_H
#define IDOL_BILEVEL_KKT_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/bilevel/optimizers/BilevelOptimizerInterface.h"
#include "idol/mixed-integer/modeling/models/KKT.h"

namespace idol::Bilevel {
    class KKT;
}

class idol::Bilevel::KKT : public OptimizerFactoryWithDefaultParameters<KKT>, public Bilevel::OptimizerInterface {
    const Bilevel::Description* m_description = nullptr;
    std::unique_ptr<OptimizerFactory> m_single_level_optimizer;
    std::unique_ptr<Reformulators::KKT::BoundProvider> m_bound_provider;
    std::optional<bool> m_use_sos1;
public:
    KKT() = default;

    explicit KKT(const Bilevel::Description& t_description);

    KKT(const Bilevel::Description& t_description, const Reformulators::KKT::BoundProvider& t_bound_provider);

    KKT(const KKT& t_src);

    void set_bilevel_description(const Description &t_bilevel_description) override;

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    KKT& with_single_level_optimizer(const OptimizerFactory& t_deterministic_optimizer);

    KKT& with_bound_provider(const Reformulators::KKT::BoundProvider& t_bound_provider);

    KKT& with_sos1_constraints(bool t_value);

    KKT& operator+=(const OptimizerFactory& t_optimizer) { return with_single_level_optimizer(t_optimizer); }

    static Model make_model(const Model& t_model, const Bilevel::Description& t_description, bool t_use_sos1 = false);

    static Model make_model(const Model& t_model, const Bilevel::Description& t_description, Reformulators::KKT::BoundProvider& t_bound_provider);
};

namespace idol {
    static Bilevel::KKT operator+(const Bilevel::KKT& t_strong_duality, const OptimizerFactory& t_optimizer) {
        Bilevel::KKT result(t_strong_duality);
        result += t_optimizer;
        return result;
    }
}

#endif //IDOL_BILEVEL_KKT_H
