//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_DECOMPOSITIONID_H
#define OPTIMIZE_DECOMPOSITIONID_H

class AbstractSolutionStrategy;
class Model;

class DecompositionId {
    AbstractSolutionStrategy* m_rmp_strategy;

    explicit DecompositionId(AbstractSolutionStrategy& t_rmp_strategy) : m_rmp_strategy(&t_rmp_strategy) {}

    friend class AbstractGenerationStrategy;
    friend class Decomposition;
};

#endif //OPTIMIZE_DECOMPOSITIONID_H
