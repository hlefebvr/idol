//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_DECOMPOSITIONID_H
#define OPTIMIZE_DECOMPOSITIONID_H

class Algorithm;
class Model;

class DecompositionId {
    Algorithm* m_rmp_strategy;

    explicit DecompositionId(Algorithm& t_rmp_strategy) : m_rmp_strategy(&t_rmp_strategy) {}

    friend class GenerationAlgorithm;
    friend class Decomposition;
};

#endif //OPTIMIZE_DECOMPOSITIONID_H
