//
// Created by henri on 24/03/23.
//
#include "optimizers/column-generation-v2/ColumnGenerationOptimizerV2.h"
#include "optimizers/column-generation-v2/ColumnGenerationV2.h"

Backend *ColumnGenerationOptimizerV2::operator()(const AbstractModel &t_model) const {

    const unsigned int n_subproblems = this->n_subproblems();

    std::vector<Model*> subproblems(n_subproblems);

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        subproblems[i] = m_subproblems[i]->clone();
    }

    return new Backends::ColumnGenerationV2(t_model.as<Model>(), &t_model.clone()->as<Model>(), subproblems, m_generation_patterns);
}

ColumnGenerationOptimizerV2 *ColumnGenerationOptimizerV2::clone() const {
    return new ColumnGenerationOptimizerV2(*this);
}

ColumnGenerationOptimizerV2::ColumnGenerationOptimizerV2(const OptimizerFactory& t_master_optimizer,
                                                         unsigned int t_n_subproblems)
    : m_master_optimizer(t_master_optimizer.clone()) {

    m_subproblems.reserve(t_n_subproblems);

}

ColumnGenerationOptimizerV2::ColumnGenerationOptimizerV2(const ColumnGenerationOptimizerV2 &t_src)
    : m_master_optimizer(t_src.m_master_optimizer->clone()),
      m_subproblems(t_src.m_subproblems),
      m_generation_patterns(t_src.m_generation_patterns) {

}

void ColumnGenerationOptimizerV2::reserve_subproblems(unsigned int t_n_subproblems) {
    m_subproblems.reserve(t_n_subproblems);
    m_generation_patterns.reserve(t_n_subproblems);
}

ColumnGenerationOptimizerV2 &ColumnGenerationOptimizerV2::with_subproblem(const Model &t_model, Column t_column) {
    m_subproblems.emplace_back(&t_model);
    m_generation_patterns.emplace_back(std::move(t_column));
    return *this;
}
