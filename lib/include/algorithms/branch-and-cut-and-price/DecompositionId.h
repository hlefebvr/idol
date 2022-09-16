//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_DECOMPOSITIONID_H
#define OPTIMIZE_DECOMPOSITIONID_H

class AbstractSolutionStrategy;
class Model;

class DecompositionId {
    AbstractSolutionStrategy* m_rmp_strategy;
    Model* m_rmp_model;

    explicit DecompositionId(Model& t_rmp_model, AbstractSolutionStrategy& t_rmp_strategy)
        : m_rmp_model(&t_rmp_model), m_rmp_strategy(&t_rmp_strategy) {}

    friend class AbstractGenerationStrategy;
    template<class> friend class DecompositionStrategy;
};

#endif //OPTIMIZE_DECOMPOSITIONID_H
