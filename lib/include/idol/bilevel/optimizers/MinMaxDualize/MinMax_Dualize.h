//
// Created by henri on 29.11.24.
//

#ifndef IDOL_BILEVEL_MINMAX_DUALIZE_H
#define IDOL_BILEVEL_MINMAX_DUALIZE_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/bilevel/optimizers/BilevelOptimizerInterface.h"
#include "idol/mixed-integer/modeling/models/KKT.h"

namespace idol::Bilevel::MinMax {
    class Dualize;
}

class idol::Bilevel::MinMax::Dualize : public OptimizerFactoryWithDefaultParameters<Dualize>, public Bilevel::OptimizerInterface {
    const Bilevel::Description* m_description = nullptr;
    std::unique_ptr<OptimizerFactory> m_single_level_optimizer;
    std::unique_ptr<Reformulators::KKT::BoundProvider> m_bound_provider;
public:
    Dualize();

    explicit Dualize(const Bilevel::Description& t_description);

    Dualize(const Dualize& t_src);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    void set_bilevel_description(const Description &t_bilevel_description) override;

    Dualize& with_single_level_optimizer(const OptimizerFactory& t_deterministic_optimizer);

    Dualize& with_bound_provider(const Reformulators::KKT::BoundProvider& t_bound_provider);

    Dualize& operator+=(const OptimizerFactory& t_optimizer) { return with_single_level_optimizer(t_optimizer); }

    static Model make_model(const Model& t_model, const Bilevel::Description& t_description);

    static Model make_model(const Model& t_model, const Bilevel::Description& t_description, ::idol::Reformulators::KKT::BoundProvider& t_bound_provider);
};

namespace idol {
    static Bilevel::MinMax::Dualize operator+(const Bilevel::MinMax::Dualize& t_MINMAX_DUALIZE, const OptimizerFactory& t_optimizer) {
        Bilevel::MinMax::Dualize result(t_MINMAX_DUALIZE);
        result += t_optimizer;
        return result;
    }
}

#endif //IDOL_BILEVEL_MINMAX_DUALIZE_H
