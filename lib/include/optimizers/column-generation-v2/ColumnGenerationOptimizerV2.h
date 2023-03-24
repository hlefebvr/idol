//
// Created by henri on 24/03/23.
//

#ifndef IDOL_COLUMNGENERATIONOPTIMIZERV2_H
#define IDOL_COLUMNGENERATIONOPTIMIZERV2_H

#include <vector>
#include "optimizers/OptimizerFactory.h"
#include "modeling/models/Model.h"

class ColumnGenerationOptimizerV2 : public OptimizerFactory {
public:
    ColumnGenerationOptimizerV2(const OptimizerFactory& t_master_optimizer, unsigned int t_n_subproblems);

    Backend *operator()(const AbstractModel &t_model) const override;

    [[nodiscard]] ColumnGenerationOptimizerV2 *clone() const override;

    void reserve_subproblems(unsigned int t_n_subproblems);

    [[nodiscard]] unsigned int n_subproblems() const { return m_subproblems.size(); }

    ColumnGenerationOptimizerV2& with_subproblem(const Model& t_model, Column t_column);
private:
    std::vector<const Model*> m_subproblems;
    std::vector<Column> m_generation_patterns;
    std::unique_ptr<OptimizerFactory> m_master_optimizer;

    ColumnGenerationOptimizerV2(const ColumnGenerationOptimizerV2& t_src);
};

#endif //IDOL_COLUMNGENERATIONOPTIMIZERV2_H
