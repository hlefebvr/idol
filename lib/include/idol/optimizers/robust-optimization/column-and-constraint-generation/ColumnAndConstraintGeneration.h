//
// Created by henri on 08.02.24.
//

#ifndef IDOL_ROBUST_COLUMNANDCONSTRAINTGENERATION_H
#define IDOL_ROBUST_COLUMNANDCONSTRAINTGENERATION_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/separators/CCGSeparator.h"
#include "idol/modeling/models/Model.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/stabilizers//CCGStabilizer.h"
#include "idol/modeling/robust-optimization/StageDescription.h"

namespace idol::Robust {
    class ColumnAndConstraintGeneration;
}

class idol::Robust::ColumnAndConstraintGeneration : public OptimizerFactoryWithDefaultParameters<ColumnAndConstraintGeneration> {
    std::unique_ptr<OptimizerFactory> m_master_optimizer;
    std::unique_ptr<CCGSeparator> m_separator;
    std::unique_ptr<CCGStabilizer> m_stabilizer;
    std::optional<bool> m_complete_recourse;

    Robust::StageDescription m_stage_description;
    const Model& m_uncertainty_set;
public:
    ColumnAndConstraintGeneration(Robust::StageDescription t_stage_description,
                                  const Model& t_uncertainty_set);

    ColumnAndConstraintGeneration(const ColumnAndConstraintGeneration& t_src);
    ColumnAndConstraintGeneration(ColumnAndConstraintGeneration&&) noexcept = default;

    ColumnAndConstraintGeneration& operator=(const ColumnAndConstraintGeneration&) = delete;
    ColumnAndConstraintGeneration& operator=(ColumnAndConstraintGeneration&&) noexcept = default;

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] ColumnAndConstraintGeneration *clone() const override;

    ColumnAndConstraintGeneration& with_master_optimizer(const OptimizerFactory& t_optimizer);

    ColumnAndConstraintGeneration& with_separator(const CCGSeparator& t_separator);

    ColumnAndConstraintGeneration& with_stabilization(const CCGStabilizer& t_stabilizer);

    ColumnAndConstraintGeneration& with_complete_recourse(bool t_value);
};

#endif //IDOL_ROBUST_COLUMNANDCONSTRAINTGENERATION_H
