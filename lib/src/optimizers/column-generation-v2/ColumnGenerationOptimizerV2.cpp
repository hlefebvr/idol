//
// Created by henri on 24/03/23.
//
#include "optimizers/column-generation-v2/ColumnGenerationOptimizerV2.h"
#include "optimizers/column-generation-v2/ColumnGenerationV2.h"

Backend *ColumnGenerationOptimizerV2::operator()(const AbstractModel &t_model) const {
    return new Backends::ColumnGenerationV2(t_model.as<Model>(), *m_master_optimizer, m_subproblems);
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
    : m_master_optimizer(t_src.m_master_optimizer->clone()) {

    m_subproblems.reserve(t_src.m_subproblems.capacity());
    const unsigned int n_subproblems = t_src.n_subproblems();

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        m_subproblems.push_back(Subproblem(t_src.m_subproblems[i]));
    }

}

void ColumnGenerationOptimizerV2::reserve_subproblems(unsigned int t_n_subproblems) {
    m_subproblems.reserve(t_n_subproblems);
}

ColumnGenerationOptimizerV2::Subproblem &
ColumnGenerationOptimizerV2::add_subproblem(const Model& t_model, Column t_column) {
    m_subproblems.emplace_back(t_model, std::move(t_column));
    return m_subproblems.back();
}

ColumnGenerationOptimizerV2 &ColumnGenerationOptimizerV2::with_subproblem(const Model &t_model, Column t_column) {
    add_subproblem(t_model, std::move(t_column));
    return *this;
}

ColumnGenerationOptimizerV2::Subproblem::Subproblem(const Model& t_model, Column&& t_column)
    : m_model(&t_model), m_generation_pattern(std::move(t_column)) {

}
