//
// Created by henri on 11.12.24.
//

#ifndef IDOL_COLUMNANDCONSTRAINTGENERATION_H
#define IDOL_COLUMNANDCONSTRAINTGENERATION_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/robust/modeling/Description.h"

namespace idol::Robust {
    class ColumnAndConstraintGeneration;
}

class idol::Robust::ColumnAndConstraintGeneration : public OptimizerFactoryWithDefaultParameters<ColumnAndConstraintGeneration> {
    const Robust::Description& m_description;
    std::unique_ptr<OptimizerFactory> m_master_optimizer;
public:
    explicit ColumnAndConstraintGeneration(const Robust::Description& t_description);

    ColumnAndConstraintGeneration(const ColumnAndConstraintGeneration& t_src);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;

    ColumnAndConstraintGeneration& with_master_optimizer(const idol::OptimizerFactory &t_deterministic_optimizer);
};

#endif //IDOL_COLUMNANDCONSTRAINTGENERATION_H
