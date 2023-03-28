//
// Created by henri on 24/03/23.
//

#ifndef IDOL_COLUMNGENERATIONOPTIMIZER_H
#define IDOL_COLUMNGENERATIONOPTIMIZER_H

#include <vector>
#include "optimizers/OptimizerFactory.h"
#include "modeling/models/Model.h"

class ColumnGenerationOptimizer : public OptimizerFactoryWithDefaultParameters<ColumnGenerationOptimizer> {
public:
    ColumnGenerationOptimizer(const OptimizerFactory& t_master_optimizer, unsigned int t_n_subproblems);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] ColumnGenerationOptimizer *clone() const override;

    void reserve_subproblems(unsigned int t_n_subproblems);

    [[nodiscard]] unsigned int n_subproblems() const { return m_subproblems.size(); }

    ColumnGenerationOptimizer& with_subproblem(const Model& t_model, Column t_column);
private:
    std::vector<const Model*> m_subproblems;
    std::vector<Column> m_generation_patterns;
    std::unique_ptr<OptimizerFactory> m_master_optimizer;

    ColumnGenerationOptimizer(const ColumnGenerationOptimizer& t_src);
};

#endif //IDOL_COLUMNGENERATIONOPTIMIZER_H
