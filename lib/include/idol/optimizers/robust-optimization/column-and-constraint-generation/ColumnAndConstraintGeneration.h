//
// Created by henri on 08.02.24.
//

#ifndef IDOL_COLUMNANDCONSTRAINTGENERATION_H
#define IDOL_COLUMNANDCONSTRAINTGENERATION_H

#include "idol/optimizers/OptimizerFactory.h"
#include "ColumnAndConstraintGenerationSeparator.h"
#include "idol/modeling/models/Model.h"

namespace idol::Robust {
    class ColumnAndConstraintGeneration;
}

class idol::Robust::ColumnAndConstraintGeneration : public OptimizerFactoryWithDefaultParameters<ColumnAndConstraintGeneration> {
    std::unique_ptr<OptimizerFactory> m_master_optimizer;
    std::unique_ptr<ColumnAndConstraintGenerationSeparator> m_separator;
    std::optional<bool> m_complete_recourse;

    const Annotation<Var, unsigned int> m_lower_level_variables;
    const Annotation<Ctr, unsigned int> m_lower_level_constraints;
    const Model& m_uncertainty_set;
public:
    ColumnAndConstraintGeneration(const Annotation<Var, unsigned int>& t_lower_level_variables,
                                  const Annotation<Ctr, unsigned int>& t_lower_level_constraints,
                                  const Model& t_uncertainty_set);

    ColumnAndConstraintGeneration(const ColumnAndConstraintGeneration& t_src);
    ColumnAndConstraintGeneration(ColumnAndConstraintGeneration&&) noexcept = default;

    ColumnAndConstraintGeneration& operator=(const ColumnAndConstraintGeneration&) = delete;
    ColumnAndConstraintGeneration& operator=(ColumnAndConstraintGeneration&&) noexcept = default;

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] ColumnAndConstraintGeneration *clone() const override;

    ColumnAndConstraintGeneration& with_master_optimizer(const OptimizerFactory& t_optimizer);

    ColumnAndConstraintGeneration& with_separator(const ColumnAndConstraintGenerationSeparator& t_separator);

    ColumnAndConstraintGeneration& with_complete_recourse(bool t_value);
};

#endif //IDOL_COLUMNANDCONSTRAINTGENERATION_H
