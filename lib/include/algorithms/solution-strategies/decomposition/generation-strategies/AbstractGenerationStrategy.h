//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_ABSTRACTGENERATIONSTRATEGY_H
#define OPTIMIZE_ABSTRACTGENERATIONSTRATEGY_H

#include "algorithms/solution-strategies/AbstractSolutionStrategy.h"
#include "algorithms/solution-strategies/decomposition/DecompositionId.h"

class AbstractGenerationStrategy : public AbstractSolutionStrategy {
    DecompositionId m_id;
protected:
    AbstractSolutionStrategy& rmp_solution_strategy();

    [[nodiscard]] const AbstractSolutionStrategy& rmp_solution_strategy() const;
public:
    explicit AbstractGenerationStrategy(DecompositionId&& t_decomposition_id);
};

#endif //OPTIMIZE_ABSTRACTGENERATIONSTRATEGY_H
