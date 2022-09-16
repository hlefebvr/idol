//
// Created by henri on 16/09/22.
//
#include "algorithms/branch-and-cut-and-price/AbstractGenerationStrategy.h"
#include "algorithms/branch-and-cut-and-price/DecompositionId.h"
#include "modeling/models/Model.h"

AbstractGenerationStrategy::AbstractGenerationStrategy(DecompositionId &&t_decomposition_id)
     : m_id(std::move(t_decomposition_id)) {

}

unsigned int AbstractGenerationStrategy::rmp_id() const {
    return m_id.m_rmp_model->id();
}

void AbstractGenerationStrategy::rmp_solve() {
    m_id.m_rmp_strategy->solve();
}

Solution::Primal AbstractGenerationStrategy::rmp_primal_solution() const {
    return m_id.m_rmp_strategy->primal_solution();
}

Solution::Dual AbstractGenerationStrategy::rmp_dual_solution() const {
    return m_id.m_rmp_strategy->dual_solution();
}

Solution::Dual AbstractGenerationStrategy::rmp_farkas_certificate() const {
    return m_id.m_rmp_strategy->farkas_certificate();
}

void AbstractGenerationStrategy::rmp_add_column(TempVar t_temporary_variable) {
    return m_id.m_rmp_strategy->add_column(t_temporary_variable);
}
