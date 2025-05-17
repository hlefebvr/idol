//
// Created by henri on 08.05.25.
//

#ifndef IDOL_ROBUST_BENDERS_H
#define IDOL_ROBUST_BENDERS_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/robust/modeling/Description.h"

namespace idol::Robust {
    class Benders;
}

class idol::Robust::Benders : public idol::OptimizerFactoryWithDefaultParameters<Benders> {
    const Bilevel::Description& m_bilevel_description;
    const Robust::Description& m_robust_description;

    // Initial scenarios
    std::vector<Point<Var>> m_initial_scenarios;

    // Separation
    std::list<std::unique_ptr<OptimizerFactory>> m_optimizer_feasibility_separation;
    std::list<std::unique_ptr<OptimizerFactory>> m_optimizer_optimality_separation;
    std::list<std::unique_ptr<OptimizerFactory>> m_optimizer_joint_separation;
public:
    Benders(const Robust::Description& t_robust_description,
            const Bilevel::Description& t_bilevel_description);

    Benders(const Benders& t_src);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    Benders& reserve_initial_scenarios(unsigned int t_n);

    Benders& add_initial_scenario(Point<Var> t_scenario);

    Benders& add_feasibility_separation_optimizer(const OptimizerFactory& t_optimizer);

    Benders& add_optimality_separation_optimizer(const OptimizerFactory& t_optimizer);

    Benders& add_joint_separation_optimizer(const OptimizerFactory& t_optimizer);
};

#endif //IDOL_ROBUST_BENDERS_H
