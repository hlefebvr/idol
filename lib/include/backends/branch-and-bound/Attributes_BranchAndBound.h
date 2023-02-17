//
// Created by henri on 17/10/22.
//

#ifndef IDOL_ATTRIBUTES_BRANCHANDBOUND_H
#define IDOL_ATTRIBUTES_BRANCHANDBOUND_H

#include "backends/parameters/Parameters.h"

IDOL_CREATE_PARAMETER_CLASS(BranchAndBound)

IDOL_CREATE_PARAMETER_TYPE(BranchAndBound, int, 2)
IDOL_CREATE_PARAMETER(BranchAndBound, int, 0, NodeSelection, 0)
IDOL_CREATE_PARAMETER(BranchAndBound, int, 1, LogFrequency, 10)

namespace NodeSelections{
    static constexpr int Automatic = 0;
    static constexpr int WorstBound = 1;
    static constexpr int BestBound = 2;
    static constexpr int DepthFirst = 3;
    static constexpr int BreadthFirst = 4;
}

#endif //IDOL_ATTRIBUTES_BRANCHANDBOUND_H
