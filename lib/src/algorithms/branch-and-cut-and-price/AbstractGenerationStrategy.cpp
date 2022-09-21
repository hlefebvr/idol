//
// Created by henri on 16/09/22.
//
#include "algorithms/solution-strategies/decomposition/generation-strategies/AbstractGenerationStrategy.h"
#include "algorithms/solution-strategies/decomposition/DecompositionId.h"
#include "modeling/models/Model.h"

AbstractGenerationStrategy::AbstractGenerationStrategy(DecompositionId &&t_decomposition_id)
     : m_id(std::move(t_decomposition_id)) {

}

AbstractSolutionStrategy &AbstractGenerationStrategy::rmp_solution_strategy() {
    return *m_id.m_rmp_strategy;
}

const AbstractSolutionStrategy &AbstractGenerationStrategy::rmp_solution_strategy() const {
    return *m_id.m_rmp_strategy;
}
