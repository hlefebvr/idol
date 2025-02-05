//
// Created by henri on 11.12.24.
//

#ifndef IDOL_COLUMNANDCONSTRAINTGENERATION_H
#define IDOL_COLUMNANDCONSTRAINTGENERATION_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/robust/modeling/Description.h"
#include "idol/bilevel/modeling/Description.h"

namespace idol::Robust {
    class ColumnAndConstraintGeneration;
}

class idol::Robust::ColumnAndConstraintGeneration : public OptimizerFactoryWithDefaultParameters<ColumnAndConstraintGeneration> {
    const Bilevel::Description& m_bilevel_description;
    const Robust::Description& m_robust_description;
    std::unique_ptr<OptimizerFactory> m_master_optimizer;

    // Initial scenarios
    std::unique_ptr<OptimizerFactory> m_initial_scenario_by_minimization;
    std::unique_ptr<OptimizerFactory> m_initial_scenario_by_maximization;
    std::vector<Point<Var>> m_initial_scenarios;

    // Separation
    std::unique_ptr<OptimizerFactory> m_optimizer_feasibility_separation;
    std::unique_ptr<OptimizerFactory> m_optimizer_optimality_separation;
public:
    ColumnAndConstraintGeneration(const Robust::Description& t_robust_description,
                                  const Bilevel::Description& t_bilevel_description);

    ColumnAndConstraintGeneration(const ColumnAndConstraintGeneration& t_src);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    ColumnAndConstraintGeneration& with_master_optimizer(const idol::OptimizerFactory &t_deterministic_optimizer);

    ColumnAndConstraintGeneration& reserve_initial_scenarios(unsigned int t_n);

    ColumnAndConstraintGeneration& add_initial_scenario(Point<Var> t_scenario);

    ColumnAndConstraintGeneration& with_initial_scenario_by_minimization(const OptimizerFactory& t_optimizer);

    ColumnAndConstraintGeneration& with_initial_scenario_by_maximization(const OptimizerFactory& t_optimizer);

    ColumnAndConstraintGeneration& add_feasibility_separation_optimizer(const OptimizerFactory& t_optimizer);

    ColumnAndConstraintGeneration& add_optimality_separation_optimizer(const OptimizerFactory& t_optimizer);
};

#endif //IDOL_COLUMNANDCONSTRAINTGENERATION_H
