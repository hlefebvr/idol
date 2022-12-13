//
// Created by henri on 12/12/22.
//

#ifndef IDOL_GENERATION_H
#define IDOL_GENERATION_H

#include <vector>
#include "algorithms/decomposition/GenerationAlgorithm.h"

class SubProblem {

};

class Generation : public GenerationAlgorithm {
    std::vector<SubProblem> m_subproblems;

public:
    Generation(Algorithm& t_rmp_solution_strategy);
};

#endif //IDOL_GENERATION_H
