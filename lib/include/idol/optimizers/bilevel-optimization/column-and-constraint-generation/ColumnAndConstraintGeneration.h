//
// Created by henri on 04.03.24.
//

#ifndef IDOL_BILEVEL_COLUMNANDCONSTRAINTGENERATION_H
#define IDOL_BILEVEL_COLUMNANDCONSTRAINTGENERATION_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/annotations/Annotation.h"
#include "idol/modeling/constraints/Ctr.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/stabilizers/CCGStabilizer.h"

namespace idol::Bilevel {
    class ColumnAndConstraintGeneration;
}

class idol::Bilevel::ColumnAndConstraintGeneration : public OptimizerFactoryWithDefaultParameters<ColumnAndConstraintGeneration>  {
    std::unique_ptr<OptimizerFactory> m_master_optimizer;
    std::unique_ptr<OptimizerFactory> m_lower_level_optimizer;
    std::unique_ptr<idol::Robust::CCGStabilizer> m_stabilizer;

    Annotation<Var, unsigned int> m_lower_level_variables;
    Annotation<Ctr, unsigned int> m_lower_level_constraints;
    Ctr m_lower_level_objective;
public:
    ColumnAndConstraintGeneration(const Annotation<Var, unsigned int>& t_lower_level_variables,
                                  const Annotation<Ctr, unsigned int>& t_lower_level_constraints,
                                  Ctr  t_lower_level_objective);

    ColumnAndConstraintGeneration(const ColumnAndConstraintGeneration& t_src);
    ColumnAndConstraintGeneration(ColumnAndConstraintGeneration&&) noexcept = default;

    ColumnAndConstraintGeneration& operator=(const ColumnAndConstraintGeneration&) = delete;
    ColumnAndConstraintGeneration& operator=(ColumnAndConstraintGeneration&&) noexcept = default;

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] ColumnAndConstraintGeneration *clone() const override;

    ColumnAndConstraintGeneration& with_master_optimizer(const OptimizerFactory& t_optimizer);

    ColumnAndConstraintGeneration& with_lower_level_optimizer(const OptimizerFactory& t_optimizer);

    ColumnAndConstraintGeneration& with_stabilization(const idol::Robust::CCGStabilizer& t_stabilizer);
};

#endif //IDOL_BILEVEL_COLUMNANDCONSTRAINTGENERATION_H
