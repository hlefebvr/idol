//
// Created by henri on 24/03/23.
//
#include "optimizers/column-generation/ColumnGenerationOptimizer.h"
#include "optimizers/column-generation/ColumnGeneration.h"

Backend *ColumnGenerationOptimizer::operator()(const AbstractModel &t_model) const {

    const unsigned int n_subproblems = this->n_subproblems();

    std::vector<Model*> subproblems(n_subproblems);

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        subproblems[i] = m_subproblems[i]->clone();
    }

    return new Backends::ColumnGeneration(t_model.as<Model>(), &t_model.clone()->as<Model>(), subproblems, m_generation_patterns);
}

ColumnGenerationOptimizer *ColumnGenerationOptimizer::clone() const {
    return new ColumnGenerationOptimizer(*this);
}

ColumnGenerationOptimizer::ColumnGenerationOptimizer(const OptimizerFactory& t_master_optimizer,
                                                         unsigned int t_n_subproblems)
    : m_master_optimizer(t_master_optimizer.clone()) {

    m_subproblems.reserve(t_n_subproblems);

}

ColumnGenerationOptimizer::ColumnGenerationOptimizer(const ColumnGenerationOptimizer &t_src)
    : m_master_optimizer(t_src.m_master_optimizer->clone()),
      m_subproblems(t_src.m_subproblems),
      m_generation_patterns(t_src.m_generation_patterns) {

}

void ColumnGenerationOptimizer::reserve_subproblems(unsigned int t_n_subproblems) {
    m_subproblems.reserve(t_n_subproblems);
    m_generation_patterns.reserve(t_n_subproblems);
}

ColumnGenerationOptimizer &ColumnGenerationOptimizer::with_subproblem(const Model &t_model, Column t_column) {
    m_subproblems.emplace_back(&t_model);
    m_generation_patterns.emplace_back(std::move(t_column));
    return *this;
}
