//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_GENERATIONALGORITHM_H
#define OPTIMIZE_GENERATIONALGORITHM_H

#include "algorithms/Algorithm.h"
#include "algorithms/decomposition/DecompositionId.h"

class GenerationAlgorithm : public Algorithm {
    DecompositionId m_id;
protected:
    Algorithm& rmp_solution_strategy();

    [[nodiscard]] const Algorithm& rmp_solution_strategy() const;
public:
    explicit GenerationAlgorithm(DecompositionId&& t_decomposition_id);
};

#endif //OPTIMIZE_GENERATIONALGORITHM_H
