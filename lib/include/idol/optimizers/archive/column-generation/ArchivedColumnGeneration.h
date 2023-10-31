//
// Created by henri on 24/03/23.
//

#ifndef IDOL_ARCHIVEDCOLUMNGENERATION_H
#define IDOL_ARCHIVEDCOLUMNGENERATION_H

#include <vector>
#include "idol/modeling/models/Model.h"
#include "OptimizerFactoryWithColumnGenerationParameters.h"

namespace idol {
    class ArchivedColumnGeneration;
}

class idol::ArchivedColumnGeneration : public impl::OptimizerFactoryWithColumnGenerationParameters<ArchivedColumnGeneration> {
public:
    explicit ArchivedColumnGeneration(unsigned int t_n_subproblems);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] ArchivedColumnGeneration *clone() const override;

    ArchivedColumnGeneration& reserve_for_subproblems(unsigned int t_n_subproblems);

    [[nodiscard]] unsigned int n_subproblems() const { return m_subproblems.size(); }

    ArchivedColumnGeneration& with_master_optimizer(const OptimizerFactory& t_master_optimizer);

    ArchivedColumnGeneration& with_subproblem(const Model& t_model, Column t_column);
private:
    std::vector<const Model*> m_subproblems;
    std::vector<Column> m_generation_patterns;
    std::unique_ptr<OptimizerFactory> m_master_optimizer;

    ArchivedColumnGeneration(const ArchivedColumnGeneration& t_src);
};

#endif //IDOL_ARCHIVEDCOLUMNGENERATION_H
