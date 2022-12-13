//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_GENERATIONALGORITHM_H
#define OPTIMIZE_GENERATIONALGORITHM_H

#include "../Algorithm.h"

class GenerationAlgorithm : public Algorithm {
    Algorithm& m_rmp_solution_strategy;
public:
    explicit GenerationAlgorithm(Algorithm& t_decomposition_id);

    Algorithm& rmp_solution_strategy();
    [[nodiscard]] const Algorithm& rmp_solution_strategy() const;
};

#endif //OPTIMIZE_GENERATIONALGORITHM_H
