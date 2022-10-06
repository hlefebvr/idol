//
// Created by henri on 16/09/22.
//
#include "algorithms/decomposition/generation-strategies/AbstractGenerationStrategy.h"
#include "algorithms/decomposition/DecompositionId.h"
#include "modeling/models/Model.h"

AbstractGenerationStrategy::AbstractGenerationStrategy(DecompositionId &&t_decomposition_id)
     : m_id(std::move(t_decomposition_id)) {

}

Algorithm &AbstractGenerationStrategy::rmp_solution_strategy() {
    return *m_id.m_rmp_strategy;
}

const Algorithm &AbstractGenerationStrategy::rmp_solution_strategy() const {
    return *m_id.m_rmp_strategy;
}
