//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_GENERATIONALGORITHM_H
#define OPTIMIZE_GENERATIONALGORITHM_H

#include "../Algorithm.h"

class GenerationAlgorithm : public Algorithm {
    Algorithm& m_rmp_solution_strategy;
protected:
    Algorithm& rmp_solution_strategy();
    [[nodiscard]] const Algorithm& rmp_solution_strategy() const;
public:
    explicit GenerationAlgorithm(Algorithm& t_decomposition_id);
};

#endif //OPTIMIZE_GENERATIONALGORITHM_H
