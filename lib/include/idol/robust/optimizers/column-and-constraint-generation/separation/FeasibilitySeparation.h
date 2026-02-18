//
// Created by Henri on 21/01/2026.
//

#ifndef IDOL_CCG_FEASIBILITYSEPARATION_H
#define IDOL_CCG_FEASIBILITYSEPARATION_H
#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/robust/optimizers/column-and-constraint-generation/separation/Separation.h"

namespace idol::Robust::CCG {
    class FeasibilitySeparation;
}

class idol::Robust::CCG::FeasibilitySeparation : public idol::Robust::CCG::Separation {
    std::unique_ptr<OptimizerFactory> m_bilevel_optimizer;
    std::optional<bool> m_with_integer_slack_variables;

    FeasibilitySeparation(const FeasibilitySeparation& t_src);
public:
    FeasibilitySeparation() = default;

    void operator()() override;
    [[nodiscard]] Separation* clone() const override { return new FeasibilitySeparation(*this); }

    FeasibilitySeparation& with_bilevel_optimizer(const OptimizerFactory& t_optimizer);

    FeasibilitySeparation& with_integer_slack_variables(bool t_value);

    std::pair<idol::Model, idol::Bilevel::Description> build_separation_problem() override;
};

#endif //IDOL_CCG_FEASIBILITYSEPARATION_H