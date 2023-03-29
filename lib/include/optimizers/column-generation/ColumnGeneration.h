//
// Created by henri on 24/03/23.
//

#ifndef IDOL_COLUMNGENERATION_H
#define IDOL_COLUMNGENERATION_H

#include <vector>
#include "modeling/models/Model.h"
#include "OptimizerFactoryWithColumnGenerationParameters.h"

class ColumnGeneration : public impl::OptimizerFactoryWithColumnGenerationParameters<ColumnGeneration> {
public:
    explicit ColumnGeneration(unsigned int t_n_subproblems);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] ColumnGeneration *clone() const override;

    void reserve_subproblems(unsigned int t_n_subproblems);

    [[nodiscard]] unsigned int n_subproblems() const { return m_subproblems.size(); }

    ColumnGeneration& with_master_solver(const OptimizerFactory& t_master_optimizer);

    ColumnGeneration& with_subproblem(const Model& t_model, Column t_column);
private:
    std::vector<const Model*> m_subproblems;
    std::vector<Column> m_generation_patterns;
    std::unique_ptr<OptimizerFactory> m_master_optimizer;

    ColumnGeneration(const ColumnGeneration& t_src);
};

#endif //IDOL_COLUMNGENERATION_H
