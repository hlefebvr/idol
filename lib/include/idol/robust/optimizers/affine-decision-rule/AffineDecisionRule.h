//
// Created by henri on 28.11.24.
//

#ifndef IDOL_AFFINEDECISIONRULE_H
#define IDOL_AFFINEDECISIONRULE_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/robust/modeling/Description.h"
#include "idol/mixed-integer/modeling/models/Model.h"

namespace idol::Robust {
    class AffineDecisionRule;
}

class idol::Robust::AffineDecisionRule : public OptimizerFactoryWithDefaultParameters<AffineDecisionRule> {
    const Robust::Description& m_description;
    std::unique_ptr<OptimizerFactory> m_deterministic_optimizer;
public:
    explicit AffineDecisionRule(const Robust::Description& t_description);

    AffineDecisionRule(const AffineDecisionRule& t_src);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    AffineDecisionRule& with_deterministic_optimizer(const OptimizerFactory& t_deterministic_optimizer);

    struct Result {

        Model model;
        std::vector<QuadExpr<Var>> affine_decision_rules;
        Robust::Description description;

        explicit Result(Model&& t_model, const Model& t_uncertainty_set) : model(std::move(t_model)), description(t_uncertainty_set) {}
    };

    static Result make_model(const Model& t_model, const Robust::Description& t_description);
};


#endif //IDOL_AFFINEDECISIONRULE_H
