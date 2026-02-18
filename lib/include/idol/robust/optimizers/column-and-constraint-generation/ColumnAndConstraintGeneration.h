//
// Created by henri on 11.12.24.
//

#ifndef IDOL_COLUMNANDCONSTRAINTGENERATION_H
#define IDOL_COLUMNANDCONSTRAINTGENERATION_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/robust/modeling/Description.h"
#include "idol/bilevel/modeling/Description.h"
#include "separation/Separation.h"

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
    std::list<std::unique_ptr<CCG::Separation>> m_separations;
    std::optional<bool> m_check_for_repeated_scenarios;
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

    ColumnAndConstraintGeneration& with_check_for_repeated_scenarios(bool t_value);

    ColumnAndConstraintGeneration& add_separation(const Robust::CCG::Separation& t_separation);

};

#endif //IDOL_COLUMNANDCONSTRAINTGENERATION_H
