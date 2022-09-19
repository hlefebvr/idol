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

AbstractSolutionStrategy &AbstractGenerationStrategy::rmp_solution_strategy() {
    return *m_id.m_rmp_strategy;
}

const AbstractSolutionStrategy &AbstractGenerationStrategy::rmp_solution_strategy() const {
    return *m_id.m_rmp_strategy;
}
